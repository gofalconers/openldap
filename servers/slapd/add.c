/* $OpenLDAP$ */
/*
 * Copyright 1998-2000 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */
/*
 * Copyright (c) 1995 Regents of the University of Michigan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of Michigan at Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#include "portable.h"

#include <stdio.h>
#include <ac/string.h>
#include <ac/time.h>
#include <ac/socket.h>

#include "ldap_pvt.h"
#include "slap.h"

static int slap_mods2entry(
	Modifications *mods,
	Entry **e,
	const char **text );

int
do_add( Connection *conn, Operation *op )
{
	BerElement	*ber = op->o_ber;
	char		*last;
	struct berval dn = { 0, NULL };
	ber_len_t	len;
	ber_tag_t	tag;
	Entry		*e;
	Backend		*be;
	LDAPModList	*modlist = NULL;
	LDAPModList	**modtail = &modlist;
	Modifications *mods = NULL;
	const char *text;
	int			rc = LDAP_SUCCESS;
	int	manageDSAit;

#ifdef NEW_LOGGING
	LDAP_LOG(( "operation", LDAP_LEVEL_ENTRY,
		   "do_add: conn %d enter\n", conn->c_connid ));
#else
	Debug( LDAP_DEBUG_TRACE, "do_add\n", 0, 0, 0 );
#endif
	/*
	 * Parse the add request.  It looks like this:
	 *
	 *	AddRequest := [APPLICATION 14] SEQUENCE {
	 *		name	DistinguishedName,
	 *		attrs	SEQUENCE OF SEQUENCE {
	 *			type	AttributeType,
	 *			values	SET OF AttributeValue
	 *		}
	 *	}
	 */

	/* get the name */
	if ( ber_scanf( ber, "{o", /*}*/ &dn ) == LBER_ERROR ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_ERR,
			"do_add: conn %d ber_scanf failed\n", conn->c_connid ));
#else
		Debug( LDAP_DEBUG_ANY, "do_add: ber_scanf failed\n", 0, 0, 0 );
#endif
		send_ldap_disconnect( conn, op,
			LDAP_PROTOCOL_ERROR, "decoding error" );
		return -1;
	}

	e = (Entry *) ch_calloc( 1, sizeof(Entry) );
	e->e_name.bv_val = NULL;
	e->e_name.bv_len = 0;
	e->e_nname.bv_val = NULL;
	e->e_nname.bv_len = 0;
	e->e_attrs = NULL;
	e->e_private = NULL;

	{
		struct berval *pdn = NULL;
		rc = dnPretty( NULL, &dn, &pdn );

		if( rc != LDAP_SUCCESS ) {
#ifdef NEW_LOGGING
			LDAP_LOG(( "operation", LDAP_LEVEL_ERR,
				"do_add: conn %d invalid dn (%s)\n", conn->c_connid,
				dn.bv_val ));
#else
			Debug( LDAP_DEBUG_ANY, "do_add: invalid dn (%s)\n", dn.bv_val, 0, 0 );
#endif
			send_ldap_result( conn, op, rc = LDAP_INVALID_DN_SYNTAX, NULL,
			    "invalid DN", NULL, NULL );
			goto done;
		}

		e->e_name = *pdn;
		free( pdn );
	}

	{
		struct berval *ndn = NULL;
		rc = dnNormalize( NULL, &dn, &ndn );

		if( rc != LDAP_SUCCESS ) {
#ifdef NEW_LOGGING
			LDAP_LOG(( "operation", LDAP_LEVEL_ERR,
				"do_add: conn %d invalid dn (%s)\n", conn->c_connid,
				dn.bv_val ));
#else
			Debug( LDAP_DEBUG_ANY, "do_add: invalid dn (%s)\n", dn.bv_val, 0, 0 );
#endif
			send_ldap_result( conn, op, rc = LDAP_INVALID_DN_SYNTAX, NULL,
			    "invalid DN", NULL, NULL );
			goto done;
		}

		e->e_nname = *ndn;
		free( ndn );
	}

#ifdef NEW_LOGGING
	LDAP_LOG(( "operation", LDAP_LEVEL_ARGS,
		"do_add: conn %d  dn (%s)\n", conn->c_connid, e->e_dn ));
#else
	Debug( LDAP_DEBUG_ARGS, "do_add: dn (%s)\n", e->e_dn, 0, 0 );
#endif

	/* get the attrs */
	for ( tag = ber_first_element( ber, &len, &last ); tag != LBER_DEFAULT;
	    tag = ber_next_element( ber, &len, last ) )
	{
		LDAPModList *mod = (LDAPModList *) ch_malloc( sizeof(LDAPModList) );
		mod->ml_op = LDAP_MOD_ADD;
		mod->ml_next = NULL;

		rc = ber_scanf( ber, "{a{V}}", &mod->ml_type, &mod->ml_bvalues );

		if ( rc == LBER_ERROR ) {
#ifdef NEW_LOGGING
			LDAP_LOG(( "operation", LDAP_LEVEL_ERR,
				   "do_add: conn %d	 decoding error \n", conn->c_connid ));
#else
			Debug( LDAP_DEBUG_ANY, "do_add: decoding error\n", 0, 0, 0 );
#endif
			send_ldap_disconnect( conn, op,
				LDAP_PROTOCOL_ERROR, "decoding error" );
			rc = -1;
			free( mod );
			goto done;
		}

		if ( mod->ml_bvalues == NULL ) {
#ifdef NEW_LOGGING
			LDAP_LOG(( "operation", LDAP_LEVEL_INFO,
				"do_add: conn %d	 no values for type %s\n",
				conn->c_connid, mod->ml_type ));
#else
			Debug( LDAP_DEBUG_ANY, "no values for type %s\n",
				mod->ml_type, 0, 0 );
#endif
			send_ldap_result( conn, op, rc = LDAP_PROTOCOL_ERROR,
				NULL, "no values for attribute type", NULL, NULL );
			free( mod->ml_type );
			free( mod );
			goto done;
		}

		*modtail = mod;
		modtail = &mod->ml_next;
	}

	if ( ber_scanf( ber, /*{*/ "}") == LBER_ERROR ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_ERR,
			"do_add: conn %d ber_scanf failed\n", conn->c_connid ));
#else
		Debug( LDAP_DEBUG_ANY, "do_add: ber_scanf failed\n", 0, 0, 0 );
#endif
		send_ldap_disconnect( conn, op,
			LDAP_PROTOCOL_ERROR, "decoding error" );
		rc = -1;
		goto done;
	}

	if( (rc = get_ctrls( conn, op, 1 )) != LDAP_SUCCESS ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_INFO,
			"do_add: conn %d get_ctrls failed\n", conn->c_connid ));
#else
		Debug( LDAP_DEBUG_ANY, "do_add: get_ctrls failed\n", 0, 0, 0 );
#endif
		goto done;
	} 

	if ( modlist == NULL ) {
		send_ldap_result( conn, op, rc = LDAP_PROTOCOL_ERROR,
			NULL, "no attributes provided", NULL, NULL );
		goto done;
	}

	Statslog( LDAP_DEBUG_STATS, "conn=%ld op=%d ADD dn=\"%s\"\n",
	    op->o_connid, op->o_opid, e->e_dn, 0, 0 );

	if( e->e_ndn == NULL || *e->e_ndn == '\0' ) {
		/* protocolError may be a more appropriate error */
		send_ldap_result( conn, op, rc = LDAP_ALREADY_EXISTS,
			NULL, "root DSE already exists",
			NULL, NULL );
		goto done;

#if defined( SLAPD_SCHEMA_DN )
	} else if ( strcasecmp( e->e_ndn, SLAPD_SCHEMA_DN ) == 0 ) {
		send_ldap_result( conn, op, rc = LDAP_ALREADY_EXISTS,
			NULL, "subschema subentry already exists",
			NULL, NULL );
		goto done;
#endif
	}

	manageDSAit = get_manageDSAit( op );

	/*
	 * We could be serving multiple database backends.  Select the
	 * appropriate one, or send a referral to our "referral server"
	 * if we don't hold it.
	 */
	be = select_backend( &e->e_nname, manageDSAit, 0 );
	if ( be == NULL ) {
		struct berval **ref = referral_rewrite( default_referral,
			NULL, e->e_dn, LDAP_SCOPE_DEFAULT );

		send_ldap_result( conn, op, rc = LDAP_REFERRAL,
			NULL, NULL, ref ? ref : default_referral, NULL );

		ber_bvecfree( ref );
		goto done;
	}

	/* check restrictions */
	rc = backend_check_restrictions( be, conn, op, NULL, &text ) ;
	if( rc != LDAP_SUCCESS ) {
		send_ldap_result( conn, op, rc,
			NULL, text, NULL, NULL );
		goto done;
	}

	/* check for referrals */
	rc = backend_check_referrals( be, conn, op, &e->e_name, &e->e_nname );
	if ( rc != LDAP_SUCCESS ) {
		goto done;
	}

	/*
	 * do the add if 1 && (2 || 3)
	 * 1) there is an add function implemented in this backend;
	 * 2) this backend is master for what it holds;
	 * 3) it's a replica and the dn supplied is the updatedn.
	 */
	if ( be->be_add ) {
		/* do the update here */
		int repl_user = be_isupdate(be, &op->o_ndn );
#ifndef SLAPD_MULTIMASTER
		if ( !be->be_update_ndn.bv_len || repl_user )
#endif
		{
			int update = be->be_update_ndn.bv_len;
			char textbuf[SLAP_TEXT_BUFLEN];
			size_t textlen = sizeof textbuf;

			rc = slap_modlist2mods( modlist, update, &mods, &text,
				textbuf, textlen );

			if( rc != LDAP_SUCCESS ) {
				send_ldap_result( conn, op, rc,
					NULL, text, NULL, NULL );
				goto done;
			}

			if ( (be->be_lastmod == ON || (be->be_lastmod == UNDEFINED &&
				global_lastmod == ON)) && !repl_user )
			{
				Modifications **modstail;
				for( modstail = &mods;
					*modstail != NULL;
					modstail = &(*modstail)->sml_next )
				{
					assert( (*modstail)->sml_op == LDAP_MOD_ADD );
					assert( (*modstail)->sml_desc != NULL );
				}
				rc = slap_mods_opattrs( op, mods, modstail, &text,
					textbuf, textlen );
				if( rc != LDAP_SUCCESS ) {
					send_ldap_result( conn, op, rc,
						NULL, text, NULL, NULL );
					goto done;
				}
			}

			rc = slap_mods2entry( mods, &e, &text );
			if( rc != LDAP_SUCCESS ) {
				send_ldap_result( conn, op, rc,
					NULL, text, NULL, NULL );
				goto done;
			}

			if ( (*be->be_add)( be, conn, op, e ) == 0 ) {
#ifdef SLAPD_MULTIMASTER
				if ( !repl_user )
#endif
				{
					replog( be, op, &e->e_name, &e->e_nname, e );
				}
				be_entry_release_w( be, conn, op, e );
				e = NULL;
			}

#ifndef SLAPD_MULTIMASTER
		} else {
			struct berval **defref = be->be_update_refs
				? be->be_update_refs : default_referral;
			struct berval **ref = referral_rewrite( defref,
				NULL, e->e_dn, LDAP_SCOPE_DEFAULT );

			send_ldap_result( conn, op, rc = LDAP_REFERRAL, NULL, NULL,
				ref ? ref : defref, NULL );

			ber_bvecfree( ref );
#endif
		}
	} else {
#ifdef NEW_LOGGING
	    LDAP_LOG(( "operation", LDAP_LEVEL_INFO,
		       "do_add: conn %d	 no backend support\n", conn->c_connid ));
#else
	    Debug( LDAP_DEBUG_ARGS, "	 do_add: no backend support\n", 0, 0, 0 );
#endif
	    send_ldap_result( conn, op, rc = LDAP_UNWILLING_TO_PERFORM,
			      NULL, "operation not supported within namingContext", NULL, NULL );
	}

done:
	free( dn.bv_val );

	if( modlist != NULL ) {
		slap_modlist_free( modlist );
	}
	if( mods != NULL ) {
		slap_mods_free( mods );
	}
	if( e != NULL ) {
		entry_free( e );
	}

	return rc;
}

static int slap_mods2entry(
	Modifications *mods,
	Entry **e,
	const char **text )
{
	Attribute **tail = &(*e)->e_attrs;
	assert( *tail == NULL );

	for( ; mods != NULL; mods = mods->sml_next ) {
		Attribute *attr;

		assert( mods->sml_op == LDAP_MOD_ADD );
		assert( mods->sml_desc != NULL );

		attr = attr_find( (*e)->e_attrs, mods->sml_desc );

		if( attr != NULL ) {
#define SLURPD_FRIENDLY
#ifdef SLURPD_FRIENDLY
			ber_len_t i,j;

			for( i=0; attr->a_vals[i]; i++ ) {
				/* count them */
			}
			for( j=0; mods->sml_bvalues[j]; j++ ) {
				/* count them */
			}
			j++;	/* NULL */
			
			attr->a_vals = ch_realloc( attr->a_vals,
				sizeof( struct berval * ) * (i+j) );

			/* should check for duplicates */
			AC_MEMCPY( &attr->a_vals[i], mods->sml_bvalues,
				sizeof( struct berval * ) * j );

			/* trim the mods array */
			ch_free( mods->sml_bvalues );
			mods->sml_bvalues = NULL;

			continue;
#else
			*text = "attribute provided more than once";
			return LDAP_TYPE_OR_VALUE_EXISTS;
#endif
		}

		attr = ch_calloc( 1, sizeof(Attribute) );

		/* move ad to attr structure */
		attr->a_desc = mods->sml_desc;
		mods->sml_desc = NULL;

		/* move values to attr structure */
		/*	should check for duplicates */
		attr->a_vals = mods->sml_bvalues;
		mods->sml_bvalues = NULL;

		*tail = attr;
		tail = &attr->a_next;
	}

	return LDAP_SUCCESS;
}
