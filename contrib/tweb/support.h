/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*                                                                          *
* support.h..                                                              *
*                                                                          *
* Function:..WorldWideWeb-X.500-Gateway - Support-Functions                *
*            Based on web500gw.c 1.3 written by Frank Richter, TU Chemmniz *
*            which is based on go500gw by Tim Howes, University of         *
*            Michigan  - All rights reserved                               *
*                                                                          *
* Authors:...Dr. Kurt Spanier & Bernhard Winkler,                          *
*            Zentrum fuer Datenverarbeitung, Bereich Entwicklung           *
*            neuer Dienste, Universitaet Tuebingen, GERMANY                *
*                                                                          *
*                                       ZZZZZ  DDD    V   V                *
*            Creation date:                Z   D  D   V   V                *
*            August 16 1995               Z    D   D   V V                 *
*            Last modification:          Z     D  D    V V                 *
*            September 7 1999           ZZZZ   DDD      V                  *
*                                                                          *
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_*/

/*
 * $Id: support.h,v 1.6 1999/09/10 15:01:20 zrnsk01 Exp $
 *
 */

#ifndef _SUPPORT_
#define _SUPPORT_

#include "support_exp.h"
#include "charray_exp.h"
#include "ch_malloc_exp.h"


/*  Array for translation */
/* 0: HEX; 1: uml (not used); 2: flatten */

char *encoding_tbl[257][3] = {
/*   0  \0 */    { "%00" ,   "",	NULL    },
/*   1   */    { "%01" ,   "",	NULL  },
/*   2   */    { "%02" ,   "",	NULL  },
/*   3   */    { "%03" ,   "",	NULL  },
/*   4   */    { "%04" ,   "",	NULL  },
/*   5   */    { "%05" ,   "",	NULL  },
/*   6   */    { "%06" ,   "",	NULL  },
/*   7   */    { "%07" ,   "",	NULL  },
/*   8   */    { "%08" ,   "",	NULL  },
/*   9  \t */    { "%09" ,   "&nbsp;",	NULL  },
/*  10  \n */    { "%0a" ,   "\n",	NULL  },
/*  11   */    { "%0b" ,   "",	NULL  },
/*  12   */    { "%0c" ,   "",	NULL  },
/*  13   */    { "%0d" ,   "",	NULL  },
/*  14   */    { "%0e" ,   "",	NULL  },
/*  15   */    { "%0f" ,   "",	NULL  },
/*  16   */    { "%10" ,   "",	NULL  },
/*  17   */    { "%11" ,   "",	NULL  },
/*  18   */    { "%12" ,   "",	NULL  },
/*  19   */    { "%13" ,   "",	NULL  },
/*  20   */    { "%14" ,   "",	NULL  },
/*  21   */    { "%15" ,   "",	NULL  },
/*  22   */    { "%16" ,   "",	NULL  },
/*  23   */    { "%17" ,   "",	NULL  },
/*  24   */    { "%18" ,   "",	NULL  },
/*  25   */    { "%19" ,   "",	NULL  },
/*  26   */    { "%1a" ,   "",	NULL  },
/*  27   */    { "%1b" ,   "",	NULL  },
/*  28   */    { "%1c" ,   "",	NULL  },
/*  29   */    { "%1d" ,   "",	NULL  },
/*  30   */    { "%1e" ,   "",	NULL  },
/*  31   */    { "%1f" ,   "",	NULL  },
/*  32     */    { "%20" ,   " ",	NULL  },
/*  33  !  */    { "%21" ,   "!",	NULL  },
/*  34  "  */    { "%22" ,   "&quot;",	NULL  },
/*  35  #  */    { "%23" ,   "#",	NULL  },
/*  36  $  */    {  NULL,    NULL,	NULL  },
/*  37  %  */    { "%25" ,   "%",	NULL  },
/*  38  &  */    { "%26" ,   "&amp;",	NULL  },
/*  39  '  */    { "%27" ,   "'",	NULL  },
/*  40  (  */    { "%28" ,   "(",	NULL  },
/*  41  )  */    { "%29" ,   ")",	NULL  },
/*  42  *  */    { "%2a" ,   "*",	NULL  },
/*  43  +  */    { "%2b" ,   "+",	NULL  },
/*  44  ,  */    { "%2c" ,   ",",	NULL  },
/*  45  -  */    {  NULL,    NULL,	NULL  },
/*  46  .  */    {  NULL,    NULL,	NULL  },
/*  47  /  */    { "%2f" ,   "/",	NULL  },
/*  48  0  */    {  NULL,    NULL,	NULL  },
/*  49  1  */    {  NULL,    NULL,	NULL  },
/*  50  2  */    {  NULL,    NULL,	NULL  },
/*  51  3  */    {  NULL,    NULL,	NULL  },
/*  52  4  */    {  NULL,    NULL,	NULL  },
/*  53  5  */    {  NULL,    NULL,	NULL  },
/*  54  6  */    {  NULL,    NULL,	NULL  },
/*  55  7  */    {  NULL,    NULL,	NULL  },
/*  56  8  */    {  NULL,    NULL,	NULL  },
/*  57  9  */    {  NULL,    NULL,	NULL  },
/*  58  :  */    { "%3a" ,   ":",	NULL  },
/*  59  ;  */    { "%3b" ,   ";",	NULL  },
/*  60  <  */    { "%3c" ,   "&lt;",	NULL  },
/*  61  =  */    { "%3d" ,   "=",	NULL  },
/*  62  >  */    { "%3e" ,   "&gt;",	NULL  },
/*  63  ?  */    { "%3f" ,   "?",	NULL  },
/*  64  @  */    { "%40" ,   "@",	NULL  },
/*  65  A  */    {  NULL,    NULL,	NULL  },
/*  66  B  */    {  NULL,    NULL,	NULL  },
/*  67  C  */    {  NULL,    NULL,	NULL  },
/*  68  D  */    {  NULL,    NULL,	NULL  },
/*  69  E  */    {  NULL,    NULL,	NULL  },
/*  70  F  */    {  NULL,    NULL,	NULL  },
/*  71  G  */    {  NULL,    NULL,	NULL  },
/*  72  H  */    {  NULL,    NULL,	NULL  },
/*  73  I  */    {  NULL,    NULL,	NULL  },
/*  74  J  */    {  NULL,    NULL,	NULL  },
/*  75  K  */    {  NULL,    NULL,	NULL  },
/*  76  L  */    {  NULL,    NULL,	NULL  },
/*  77  M  */    {  NULL,    NULL,	NULL  },
/*  78  N  */    {  NULL,    NULL,	NULL  },
/*  79  O  */    {  NULL,    NULL,	NULL  },
/*  80  P  */    {  NULL,    NULL,	NULL  },
/*  81  Q  */    {  NULL,    NULL,	NULL  },
/*  82  R  */    {  NULL,    NULL,	NULL  },
/*  83  S  */    {  NULL,    NULL,	NULL  },
/*  84  T  */    {  NULL,    NULL,	NULL  },
/*  85  U  */    {  NULL,    NULL,	NULL  },
/*  86  V  */    {  NULL,    NULL,	NULL  },
/*  87  W  */    {  NULL,    NULL,	NULL  },
/*  88  X  */    {  NULL,    NULL,	NULL  },
/*  89  Y  */    {  NULL,    NULL,	NULL  },
/*  90  Z  */    {  NULL,    NULL,	NULL  },
/*  91  [  */    { "%5b" ,   "[",	NULL  },
/*  92  \  */    { "%5c" ,   "\\",	NULL  },
/*  93  ]  */    { "%5d" ,   "]",	NULL  },
/*  94  ^  */    { "%5e" ,   "^",	NULL  },
/*  95  _  */    {  NULL,    NULL,	NULL  },
/*  96  `  */    { "%60" ,   "`",	NULL  },
/*  97  a  */    {  NULL,    NULL,	NULL  },
/*  98  b  */    {  NULL,    NULL,	NULL  },
/*  99  c  */    {  NULL,    NULL,	NULL  },
/* 100  d  */    {  NULL,    NULL,	NULL  },
/* 101  e  */    {  NULL,    NULL,	NULL  },
/* 102  f  */    {  NULL,    NULL,	NULL  },
/* 103  g  */    {  NULL,    NULL,	NULL  },
/* 104  h  */    {  NULL,    NULL,	NULL  },
/* 105  i  */    {  NULL,    NULL,	NULL  },
/* 106  j  */    {  NULL,    NULL,	NULL  },
/* 107  k  */    {  NULL,    NULL,	NULL  },
/* 108  l  */    {  NULL,    NULL,	NULL  },
/* 109  m  */    {  NULL,    NULL,	NULL  },
/* 110  n  */    {  NULL,    NULL,	NULL  },
/* 111  o  */    {  NULL,    NULL,	NULL  },
/* 112  p  */    {  NULL,    NULL,	NULL  },
/* 113  q  */    {  NULL,    NULL,	NULL  },
/* 114  r  */    {  NULL,    NULL,	NULL  },
/* 115  s  */    {  NULL,    NULL,	NULL  },
/* 116  t  */    {  NULL,    NULL,	NULL  },
/* 117  u  */    {  NULL,    NULL,	NULL  },
/* 118  v  */    {  NULL,    NULL,	NULL  },
/* 119  w  */    {  NULL,    NULL,	NULL  },
/* 120  x  */    {  NULL,    NULL,	NULL  },
/* 121  y  */    {  NULL,    NULL,	NULL  },
/* 122  z  */    {  NULL,    NULL,	NULL  },
/* 123  {  */    { "%7b" ,   "{",	NULL  },
/* 124  |  */    { "%7c" ,   "|",	NULL  },
/* 125  }  */    { "%7d" ,   "}",	NULL  },
/* 126  ~  */    { "%7e" ,   "~",	NULL  },
/* 127   */    { "%7f" ,   "",	NULL  },
/* 128  �   */    { "%80" ,   "�",	NULL  },
/* 129  �   */   { "%81" ,   "�",	NULL  },
/* 130  �   */   { "%82" ,   "�",	NULL  },
/* 131  �   */   { "%83" ,   "�",	NULL  },
/* 132  �   */   { "%84" ,   "�",	NULL  },
/* 133  �   */   { "%85" ,   "�",	NULL  },
/* 134  �   */   { "%86" ,   "�",	NULL  },
/* 135  �   */   { "%87" ,   "�",	NULL  },
/* 136  �   */   { "%88" ,   "�",	NULL  },
/* 137  �   */   { "%89" ,   "�",	NULL  },
/* 138  �   */   { "%8a" ,   "�",	NULL  },
/* 139  �   */   { "%8b" ,   "�",	NULL  },
/* 140  �   */   { "%8c" ,   "�",	NULL  },
/* 141  �   */   { "%8d" ,   "�",	NULL  },
/* 142  �   */   { "%8e" ,   "�",	NULL  },
/* 143  �   */   { "%8f" ,   "�",	NULL  },
/* 144  �   */   { "%90" ,   "�",	NULL  },
/* 145  �   */   { "%91" ,   "�",	NULL  },
/* 146  �   */   { "%92" ,   "�",	NULL  },
/* 147  �   */   { "%93" ,   "�",	NULL  },
/* 148  �   */   { "%94" ,   "�",	NULL  },
/* 149  �   */   { "%95" ,   "�",	NULL  },
/* 150  �   */   { "%96" ,   "�",	NULL  },
/* 151  �   */   { "%97" ,   "�",	NULL  },
/* 152  �   */   { "%98" ,   "�",	NULL  },
/* 153  �   */   { "%99" ,   "�",	NULL  },
/* 154  �   */   { "%9a" ,   "�",	NULL  },
/* 155  �   */   { "%9b" ,   "�",	NULL  },
/* 156  �   */   { "%9c" ,   "�",	NULL  },
/* 157  �   */   { "%9d" ,   "�",	NULL  },
/* 158  �   */   { "%9e" ,   "�",	NULL  },
/* 159  �   */   { "%9f" ,   "�",	NULL  },
/* 160     */    { "%a0" ,   "",	NULL  },
/* 161     */    { "%a1" ,   "",	NULL  },
/* 162     */    { "%a2" ,   "",	NULL  },
/* 163     */    { "%a3" ,   "",	NULL  },
/* 164     */    { "%a4" ,   "",	NULL  },
/* 165     */    { "%a5" ,   "",	NULL  },
/* 166     */    { "%a6" ,   "",	NULL  },
/* 167     */    { "%a7" ,   "",	NULL  },
/* 168     */    { "%a8" ,   "",	NULL  },
/* 169     */    { "%a9" ,   "",	NULL  },
/* 170     */    { "%aa" ,   "",	NULL  },
/* 171     */    { "%ab" ,   "",	NULL  },
/* 172     */    { "%ac" ,   "",	NULL  },
/* 173     */    { "%ad" ,   "",	NULL  },
/* 174     */    { "%ae" ,   "",	NULL  },
/* 175     */    { "%af" ,   "",	NULL  },
/* 176     */    { "%b0" ,   "",	NULL  },
/* 177     */    { "%b1" ,   "",	NULL  },
/* 178     */    { "%b2" ,   "",	NULL  },
/* 179     */    { "%b3" ,   "",	NULL  },
/* 180     */    { "%b4" ,   "",	NULL  },
/* 181     */    { "%b5" ,   "",	NULL  },
/* 182     */    { "%b6" ,   "",	NULL  },
/* 183     */    { "%b7" ,   "",	NULL  },
/* 184     */    { "%b8" ,   "",	NULL  },
/* 185     */    { "%b9" ,   "",	NULL  },
/* 186     */    { "%ba" ,   "",	NULL  },
/* 187     */    { "%bb" ,   "",	NULL  },
/* 188     */    { "%bc" ,   "",	NULL  },
/* 189     */    { "%bd" ,   "",	NULL  },
/* 190     */    { "%be" ,   "",	NULL  },
/* 191     */    { "%bf" ,   "",	NULL  },
/* 192     */    { "%c0" ,   "&Agrave;",    "A"  },
/* 193     */    { "%c1" ,   "&Aacute;",    "A"  },
/* 194     */    { "%c2" ,   "&Acirc;",     "A"  },
/* 195     */    { "%c3" ,   "&Atilde;",    "A"  },
/* 196     */    { "%c4" ,   "&Auml;",      "Ae"  },
/* 197     */    { "%c5" ,   "&Aring;",     "A"  },
/* 198     */    { "%c6" ,   "&AElig;",     "AE"  },
/* 199     */    { "%c7" ,   "&Ccedil;",    "C"  },
/* 200     */    { "%c8" ,   "&Egrave;",    "E"  },
/* 201     */    { "%c9" ,   "&Eacute;",    "E"  },
/* 202     */    { "%ca" ,   "&Ecirc;",     "E"  },
/* 203     */    { "%cb" ,   "&Euml;",      "E"  },
/* 204     */    { "%cc" ,   "&Igrave;",    "I"  },
/* 205     */    { "%cd" ,   "&Iacute;",    "I"  },
/* 206     */    { "%ce" ,   "&Icirc;",     "I"  },
/* 207     */    { "%cf" ,   "&Iuml;",      "I"  },
/* 208     */    { "%d0" ,   "&ETH;",       "Eth"  },
/* 209     */    { "%d1" ,   "&Ntilde;",    "N"  },
/* 210     */    { "%d2" ,   "&Ograve;",    "O"  },
/* 211     */    { "%d3" ,   "&Oacute;",    "O"  },
/* 212     */    { "%d4" ,   "&Ocirc;",     "O"  },
/* 213     */    { "%d5" ,   "&Otilde;",    "O"  },
/* 214     */    { "%d6" ,   "&Ouml;",      "Oe"  },
/* 215     */    { "%d7" ,   "x",           "x"  },
/* 216     */    { "%d8" ,   "&Oslash;",    "O"  },
/* 217     */    { "%d9" ,   "&Ugrave;",    "U"  },
/* 218     */    { "%da" ,   "&Uacute;",    "U"  },
/* 219     */    { "%db" ,   "&Ucirc;",     "U"  },
/* 220     */    { "%dc" ,   "&Uuml;",      "Ue"  },
/* 221     */    { "%dd" ,   "&Yacute;",    "Y"  },
/* 222     */    { "%de" ,   "&THORN;",     NULL  },
/* 223     */    { "%df" ,   "&szlig;",     "ss"  },
/* 224     */    { "%e0" ,   "&agrave;",    "a"  },
/* 225     */    { "%e1" ,   "&aacute;",    "a"  },
/* 226     */    { "%e2" ,   "&acirc;",     "a"  },
/* 227     */    { "%e3" ,   "&atilde;",    "a"  },
/* 228     */    { "%e4" ,   "&auml;",      "ae"  },
/* 229     */    { "%e5" ,   "&aring;",     "a"  },
/* 230     */    { "%e6" ,   "&aelig;",     "ae"  },
/* 231     */    { "%e7" ,   "&ccedil;",    "c"  },
/* 232     */    { "%e8" ,   "&egrave;",    "e"  },
/* 233     */    { "%e9" ,   "&eacute;",    "e"  },
/* 234     */    { "%ea" ,   "&ecirc;",     "e"  },
/* 235     */    { "%eb" ,   "&euml;",      "e"  },
/* 236     */    { "%ec" ,   "&igrave;",    "i"  },
/* 237     */    { "%ed" ,   "&iacute;",    "i"  },
/* 238     */    { "%ee" ,   "&icirc;",     "i"  },
/* 239     */    { "%ef" ,   "&iuml;",      "i"  },
/* 240     */    { "%f0" ,   "&eth;",       "eth"  },
/* 241     */    { "%f1" ,   "&ntilde;",    "n"  },
/* 242     */    { "%f2" ,   "&ograve;",    "o"  },
/* 243     */    { "%f3" ,   "&oacute;",    "o"  },
/* 244     */    { "%f4" ,   "&ocirc;",     "o"  },
/* 245     */    { "%f5" ,   "&otilde;",    "o"  },
/* 246     */    { "%f6" ,   "&ouml;",      "oe"  },
/* 247     */    { "%f7" ,   "/",           "/"  },
/* 248     */    { "%f8" ,   "&oslash;",    "o"  },
/* 249     */    { "%f9" ,   "&ugrave;",    "u"  },
/* 250     */    { "%fa" ,   "&uacute;",    "u"  },
/* 251     */    { "%fb" ,   "&ucirc;",     "u"  },
/* 252     */    { "%fc" ,   "&uuml;",      "ue"  },
/* 253     */    { "%fd" ,   "&yacute;",    "y"  },
/* 254     */    { "%fe" ,   "&thorn;",     NULL  },
/* 255     */    { "%ff" ,   "&yuml;",      "y"  },
/* 256  EOF*/    {  NULL,    NULL,          NULL  },
};

#endif /* _SUPPORT_ */


