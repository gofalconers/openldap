/* dn2id.c - routines to deal with the dn2id index */
/* $OpenLDAP$ */
/*
 * Copyright 1998-2000 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "back-ldbm.h"
#include "proto-back-ldbm.h"

int
dn2id_add(
    Backend	*be,
    const char	*dn,
    ID		id
)
{
	int		rc, flags;
	DBCache	*db;
	Datum		key, data;
	struct ldbminfo *li = (struct ldbminfo *) be->be_private;

	Debug( LDAP_DEBUG_TRACE, "=> dn2id_add( \"%s\", %ld )\n", dn, id, 0 );
	assert( id != NOID );

	if ( (db = ldbm_cache_open( be, "dn2id", LDBM_SUFFIX, LDBM_WRCREAT ))
	    == NULL ) {
		Debug( LDAP_DEBUG_ANY, "Could not open/create dn2id%s\n",
		    LDBM_SUFFIX, 0, 0 );
		return( -1 );
	}

	ldbm_datum_init( key );
	key.dsize = strlen( dn ) + 2;
	key.dptr = ch_malloc( key.dsize );
	sprintf( key.dptr, "%c%s", DN_BASE_PREFIX, dn );

	ldbm_datum_init( data );
	data.dptr = (char *) &id;
	data.dsize = sizeof(ID);

	flags = LDBM_INSERT;
	rc = ldbm_cache_store( db, key, data, flags );

	free( key.dptr );

	if ( rc != -1 ) {
		char *pdn = dn_parent( NULL, dn );

		if( pdn != NULL ) {
			ldbm_datum_init( key );
			key.dsize = strlen( pdn ) + 2;
			key.dptr = ch_malloc( key.dsize );
			sprintf( key.dptr, "%c%s", DN_ONE_PREFIX, pdn );
			ldap_pvt_thread_mutex_lock( &db->dbc_write_mutex );
			rc = idl_insert_key( be, db, key, id );
			ldap_pvt_thread_mutex_unlock( &db->dbc_write_mutex );
			free( key.dptr );
			free( pdn );
		}
	}

	if ( rc != -1 ) {
		char **subtree = dn_subtree( NULL, dn );

		if( subtree != NULL ) {
			int i;
			for( i=0; subtree[i] != NULL; i++ ) {
				ldbm_datum_init( key );
				key.dsize = strlen( subtree[i] ) + 2;
				key.dptr = ch_malloc( key.dsize );
				sprintf( key.dptr, "%c%s",
					DN_SUBTREE_PREFIX, subtree[i] );
				ldap_pvt_thread_mutex_lock( &db->dbc_write_mutex );
				rc = idl_insert_key( be, db, key, id );
				ldap_pvt_thread_mutex_unlock( &db->dbc_write_mutex );
				free( key.dptr );

				if(rc == -1) break;
			}

			charray_free( subtree );
		}
	}

	ldbm_cache_close( be, db );

	Debug( LDAP_DEBUG_TRACE, "<= dn2id_add %d\n", rc, 0, 0 );
	return( rc );
}

int
dn2id(
    Backend	*be,
    const char	*dn,
    ID          *idp
)
{
	struct ldbminfo	*li = (struct ldbminfo *) be->be_private;
	DBCache	*db;
	Datum		key, data;

	Debug( LDAP_DEBUG_TRACE, "=> dn2id( \"%s\" )\n", dn, 0, 0 );

	assert( idp );

	/* first check the cache */
	if ( (*idp = cache_find_entry_ndn2id( be, &li->li_cache, dn )) != NOID ) {
		Debug( LDAP_DEBUG_TRACE, "<= dn2id %ld (in cache)\n", *idp,
			0, 0 );
		return( 0 );
	}

	if ( (db = ldbm_cache_open( be, "dn2id", LDBM_SUFFIX, LDBM_WRCREAT ))
		== NULL ) {
		Debug( LDAP_DEBUG_ANY, "<= dn2id could not open dn2id%s\n",
			LDBM_SUFFIX, 0, 0 );
		/*
		 * return code !0 if ldbm cache open failed;
		 * callers should handle this
		 */
		*idp = NOID;
		return( -1 );
	}

	ldbm_datum_init( key );

	key.dsize = strlen( dn ) + 2;
	key.dptr = ch_malloc( key.dsize );
	sprintf( key.dptr, "%c%s", DN_BASE_PREFIX, dn );

	data = ldbm_cache_fetch( db, key );

	ldbm_cache_close( be, db );

	free( key.dptr );

	if ( data.dptr == NULL ) {
		Debug( LDAP_DEBUG_TRACE, "<= dn2id NOID\n", 0, 0, 0 );
		*idp = NOID;
		return( 0 );
	}

	AC_MEMCPY( (char *) idp, data.dptr, sizeof(ID) );

	assert( *idp != NOID );

	ldbm_datum_free( db->dbc_db, data );

	Debug( LDAP_DEBUG_TRACE, "<= dn2id %ld\n", *idp, 0, 0 );

	return( 0 );
}

int
dn2idl(
    Backend	*be,
    const char	*dn,
    int		prefix,
    ID_BLOCK    **idlp
)
{
	DBCache	*db;
	Datum		key;

	Debug( LDAP_DEBUG_TRACE, "=> dn2idl( \"%c%s\" )\n", prefix, dn, 0 );

	assert( idlp != NULL );
	*idlp = NULL;

	if ( (db = ldbm_cache_open( be, "dn2id", LDBM_SUFFIX, LDBM_WRCREAT ))
		== NULL ) {
		Debug( LDAP_DEBUG_ANY, "<= dn2idl could not open dn2id%s\n",
			LDBM_SUFFIX, 0, 0 );
		return -1;
	}

	ldbm_datum_init( key );

	key.dsize = strlen( dn ) + 2;
	key.dptr = ch_malloc( key.dsize );
	sprintf( key.dptr, "%c%s", prefix, dn );

	*idlp = idl_fetch( be, db, key );

	ldbm_cache_close( be, db );

	free( key.dptr );

	return( 0 );
}


int
dn2id_delete(
    Backend	*be,
    const char	*dn,
	ID id
)
{
	DBCache	*db;
	Datum		key;
	int		rc;

	Debug( LDAP_DEBUG_TRACE, "=> dn2id_delete( \"%s\", %ld )\n", dn, id, 0 );

	assert( id != NOID );

	if ( (db = ldbm_cache_open( be, "dn2id", LDBM_SUFFIX, LDBM_WRCREAT ))
	    == NULL ) {
		Debug( LDAP_DEBUG_ANY,
		    "<= dn2id_delete could not open dn2id%s\n", LDBM_SUFFIX,
		    0, 0 );
		return( -1 );
	}


	{
		char *pdn = dn_parent( NULL, dn );

		if( pdn != NULL ) {
			ldbm_datum_init( key );
			key.dsize = strlen( pdn ) + 2;
			key.dptr = ch_malloc( key.dsize );
			sprintf( key.dptr, "%c%s", DN_ONE_PREFIX, pdn );

			ldap_pvt_thread_mutex_lock( &db->dbc_write_mutex );
			(void) idl_delete_key( be, db, key, id );
			ldap_pvt_thread_mutex_unlock( &db->dbc_write_mutex );

			free( key.dptr );
			free( pdn );
		}
	}

	{
		char **subtree = dn_subtree( NULL, dn );

		if( subtree != NULL ) {
			int i;
			for( i=0; subtree[i] != NULL; i++ ) {
				ldbm_datum_init( key );
				key.dsize = strlen( subtree[i] ) + 2;
				key.dptr = ch_malloc( key.dsize );
				sprintf( key.dptr, "%c%s",
					DN_SUBTREE_PREFIX, subtree[i] );

				ldap_pvt_thread_mutex_lock( &db->dbc_write_mutex );
				(void) idl_delete_key( be, db, key, id );
				ldap_pvt_thread_mutex_unlock( &db->dbc_write_mutex );

				free( key.dptr );
			}

			charray_free( subtree );
		}
	}

	ldbm_datum_init( key );

	key.dsize = strlen( dn ) + 2;
	key.dptr = ch_malloc( key.dsize );
	sprintf( key.dptr, "%c%s", DN_BASE_PREFIX, dn );

	rc = ldbm_cache_delete( db, key );

	free( key.dptr );

	ldbm_cache_close( be, db );

	Debug( LDAP_DEBUG_TRACE, "<= dn2id_delete %d\n", rc, 0, 0 );
	return( rc );
}

/*
 * dn2entry - look up dn in the cache/indexes and return the corresponding
 * entry.
 */

Entry *
dn2entry_rw(
    Backend	*be,
    const char	*dn,
    Entry	**matched,
    int         rw
)
{
	ID		id;
	Entry		*e = NULL;
	char		*pdn;

	Debug(LDAP_DEBUG_TRACE, "dn2entry_%s: dn: \"%s\"\n",
		rw ? "w" : "r", dn, 0);

	if( matched != NULL ) {
		/* caller cares about match */
		*matched = NULL;
	}

	if ( dn2id( be, dn, &id ) ) {
		/* something bad happened to ldbm cache */
		return( NULL );

	} else if ( id != NOID ) {
		/* try to return the entry */
		if ((e = id2entry_rw( be, id, rw )) != NULL ) {
			return( e );
		}

		Debug(LDAP_DEBUG_ANY,
			"dn2entry_%s: no entry for valid id (%ld), dn \"%s\"\n",
			rw ? "w" : "r", id, dn);
		/* must have been deleted from underneath us */
		/* treat as if NOID was found */
	}

	/* caller doesn't care about match */
	if( matched == NULL ) return NULL;

	/* entry does not exist - see how much of the dn does exist */
	/* dn_parent checks returns NULL if dn is suffix */
	if ( (pdn = dn_parent( be, dn )) != NULL ) {
		/* get entry with reader lock */
		if ( (e = dn2entry_r( be, pdn, matched )) != NULL ) {
			*matched = e;
		}
		free( pdn );
	}

	return NULL;
}

