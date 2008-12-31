/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
 * HTML entity resources:
 *
 * http://msdn.microsoft.com/workshop/author/dhtml/reference/charsets/charset2.asp
 * http://msdn.microsoft.com/workshop/author/dhtml/reference/charsets/charset3.asp
 * http://www.unicode.org/Public/MAPPINGS/OBSOLETE/UNI2SGML.TXT
 *
 * http://www.w3.org/TR/2002/REC-xhtml1-20020801/dtds.html#h-A2
 * 
 */

#include "php.h"
#if PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif
#include "reg.h"
#include "html.h"
#include "php_string.h"
#include "SAPI.h"
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#if HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#if HAVE_MBSTRING
# include "ext/mbstring/mbstring.h"
ZEND_EXTERN_MODULE_GLOBALS(mbstring)
#endif

enum entity_charset { cs_terminator, cs_8859_1, cs_cp1252,
					  cs_8859_15, cs_utf_8, cs_big5, cs_gb2312, 
					  cs_big5hkscs, cs_sjis, cs_eucjp, cs_koi8r,
					  cs_cp1251, cs_8859_5, cs_cp866, cs_macroman
					};
typedef const char *const entity_table_t;

/* codepage 1252 is a Windows extension to iso-8859-1. */
static entity_table_t ent_cp_1252[] = {
	"euro", NULL, "sbquo", "fnof", "bdquo", "hellip", "dagger",
	"Dagger", "circ", "permil", "Scaron", "lsaquo", "OElig",
	NULL, NULL, NULL, NULL, "lsquo", "rsquo", "ldquo", "rdquo",
	"bull", "ndash", "mdash", "tilde", "trade", "scaron", "rsaquo",
	"oelig", NULL, NULL, "Yuml" 
};

static entity_table_t ent_iso_8859_1[] = {
	"nbsp", "iexcl", "cent", "pound", "curren", "yen", "brvbar",
	"sect", "uml", "copy", "ordf", "laquo", "not", "shy", "reg",
	"macr", "deg", "plusmn", "sup2", "sup3", "acute", "micro",
	"para", "middot", "cedil", "sup1", "ordm", "raquo", "frac14",
	"frac12", "frac34", "iquest", "Agrave", "Aacute", "Acirc",
	"Atilde", "Auml", "Aring", "AElig", "Ccedil", "Egrave",
	"Eacute", "Ecirc", "Euml", "Igrave", "Iacute", "Icirc",
	"Iuml", "ETH", "Ntilde", "Ograve", "Oacute", "Ocirc", "Otilde",
	"Ouml", "times", "Oslash", "Ugrave", "Uacute", "Ucirc", "Uuml",
	"Yacute", "THORN", "szlig", "agrave", "aacute", "acirc",
	"atilde", "auml", "aring", "aelig", "ccedil", "egrave",
	"eacute", "ecirc", "euml", "igrave", "iacute", "icirc",
	"iuml", "eth", "ntilde", "ograve", "oacute", "ocirc", "otilde",
	"ouml", "divide", "oslash", "ugrave", "uacute", "ucirc",
	"uuml", "yacute", "thorn", "yuml"
};

static entity_table_t ent_iso_8859_15[] = {
	"nbsp", "iexcl", "cent", "pound", "euro", "yen", "Scaron",
	"sect", "scaron", "copy", "ordf", "laquo", "not", "shy", "reg",
	"macr", "deg", "plusmn", "sup2", "sup3", NULL, /* Zcaron */
	"micro", "para", "middot", NULL, /* zcaron */ "sup1", "ordm",
	"raquo", "OElig", "oelig", "Yuml", "iquest", "Agrave", "Aacute",
	"Acirc", "Atilde", "Auml", "Aring", "AElig", "Ccedil", "Egrave",
	"Eacute", "Ecirc", "Euml", "Igrave", "Iacute", "Icirc",
	"Iuml", "ETH", "Ntilde", "Ograve", "Oacute", "Ocirc", "Otilde",
	"Ouml", "times", "Oslash", "Ugrave", "Uacute", "Ucirc", "Uuml",
	"Yacute", "THORN", "szlig", "agrave", "aacute", "acirc",
	"atilde", "auml", "aring", "aelig", "ccedil", "egrave",
	"eacute", "ecirc", "euml", "igrave", "iacute", "icirc",
	"iuml", "eth", "ntilde", "ograve", "oacute", "ocirc", "otilde",
	"ouml", "divide", "oslash", "ugrave", "uacute", "ucirc",
	"uuml", "yacute", "thorn", "yuml"
};

static entity_table_t ent_uni_338_402[] = {
	/* 338 (0x0152) */
	"OElig", "oelig", NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 352 (0x0160) */
	"Scaron", "scaron", NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 376 (0x0178) */
	"Yuml", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 400 (0x0190) */
	NULL, NULL, "fnof"
};

static entity_table_t ent_uni_spacing[] = {
	/* 710 */
	"circ",
	/* 711 - 730 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 731 - 732 */
	NULL, "tilde"
};

static entity_table_t ent_uni_greek[] = {
	/* 913 */
	"Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta",
	"Iota", "Kappa", "Lambda", "Mu", "Nu", "Xi", "Omicron", "Pi", "Rho",
	NULL, "Sigma", "Tau", "Upsilon", "Phi", "Chi", "Psi", "Omega",
	/* 938 - 944 are not mapped */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta",
	"iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi", "rho",
	"sigmaf", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega",
	/* 970 - 976 are not mapped */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"thetasym", "upsih",
	NULL, NULL, NULL,
	"piv" 
};

static entity_table_t ent_uni_punct[] = {
	/* 8194 */
	"ensp", "emsp", NULL, NULL, NULL, NULL, NULL,
	"thinsp", NULL, NULL, "zwnj", "zwj", "lrm", "rlm",
	NULL, NULL, NULL, "ndash", "mdash", NULL, NULL, NULL,
	/* 8216 */
	"lsquo", "rsquo", "sbquo", NULL, "ldquo", "rdquo", "bdquo", NULL,
	"dagger", "Dagger",	"bull", NULL, NULL, NULL, "hellip",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "permil", NULL,
	/* 8242 */
	"prime", "Prime", NULL, NULL, NULL, NULL, NULL, "lsaquo", "rsaquo", NULL,
	NULL, NULL, "oline", NULL, NULL, NULL, NULL, NULL,
	"frasl"
};

static entity_table_t ent_uni_euro[] = {
	"euro"
};

static entity_table_t ent_uni_8465_8501[] = {
	/* 8465 */
	"image", NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8472 */
	"weierp", NULL, NULL, NULL,
	/* 8476 */
	"real", NULL, NULL, NULL, NULL, NULL,
	/* 8482 */
	"trade", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8501 */
	"alefsym",
};

static entity_table_t ent_uni_8592_9002[] = {
	/* 8592 (0x2190) */
	"larr", "uarr", "rarr", "darr", "harr", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8608 (0x21a0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8624 (0x21b0) */
	NULL, NULL, NULL, NULL, NULL, "crarr", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8640 (0x21c0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8656 (0x21d0) */
	"lArr", "uArr", "rArr", "dArr", "hArr", "vArr", NULL, NULL,
	NULL, NULL, "lAarr", "rAarr", NULL, "rarrw", NULL, NULL,
	/* 8672 (0x21e0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8704 (0x2200) */
	"forall", "comp", "part", "exist", "nexist", "empty", NULL, "nabla",
	"isin", "notin", "epsis", "ni", "notni", "bepsi", NULL, "prod",
	/* 8720 (0x2210) */
	"coprod", "sum", "minus", "mnplus", "plusdo", NULL, "setmn", "lowast",
	"compfn", NULL, "radic", NULL, NULL, "prop", "infin", "ang90",
	/* 8736 (0x2220) */
	"ang", "angmsd", "angsph", "mid", "nmid", "par", "npar", "and",
	"or", "cap", "cup", "int", NULL, NULL, "conint", NULL,
	/* 8752 (0x2230) */
	NULL, NULL, NULL, NULL, "there4", "becaus", NULL, NULL,
	NULL, NULL, NULL, NULL, "sim", "bsim", NULL, NULL,
	/* 8768 (0x2240) */
	"wreath", "nsim", NULL, "sime", "nsime", "cong", NULL, "ncong",
	"asymp", "nap", "ape", NULL, "bcong", "asymp", "bump", "bumpe",
	/* 8784 (0x2250) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8800 (0x2260) */
	"ne", "equiv", NULL, NULL, "le", "ge", "lE", "gE",
	"lnE", "gnE", "Lt", "Gt", "twixt", NULL, "nlt", "ngt",
	/* 8816 (0x2270) */
	"nles", "nges", "lsim", "gsim", NULL, NULL, "lg", "gl",
	NULL, NULL, "pr", "sc", "cupre", "sscue", "prsim", "scsim",
	/* 8832 (0x2280) */
	"npr", "nsc", "sub", "sup", "nsub", "nsup", "sube", "supe",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8848 (0x2290) */
	NULL, NULL, NULL, NULL, NULL, "oplus", NULL, "otimes",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8864 (0x22a0) */
	NULL, NULL, NULL, NULL, NULL, "perp", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8880 (0x22b0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8896 (0x22c0) */
	NULL, NULL, NULL, NULL, NULL, "sdot", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8912 (0x22d0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8928 (0x22e0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8944 (0x22f0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8960 (0x2300) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"lceil", "rceil", "lfloor", "rfloor", NULL, NULL, NULL, NULL,
	/* 8976 (0x2310) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8992 (0x2320) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, "lang", "rang"
};

static entity_table_t ent_uni_9674[] = {
	/* 9674 */
	"loz"
};

static entity_table_t ent_uni_9824_9830[] = {
	/* 9824 */
	"spades", NULL, NULL, "clubs", NULL, "hearts", "diams"
};

static entity_table_t ent_koi8r[] = {
	"#1105", /* "jo "*/
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, "#1025", /* "JO" */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	"#1102", "#1072", "#1073", "#1094", "#1076", "#1077", "#1092", 
	"#1075", "#1093", "#1080", "#1081", "#1082", "#1083", "#1084", 
	"#1085", "#1086", "#1087", "#1103", "#1088", "#1089", "#1090", 
	"#1091", "#1078", "#1074", "#1100", "#1099", "#1079", "#1096", 
	"#1101", "#1097", "#1095", "#1098", "#1070", "#1040", "#1041", 
	"#1062", "#1044", "#1045", "#1060", "#1043", "#1061", "#1048", 
	"#1049", "#1050", "#1051", "#1052", "#1053", "#1054", "#1055", 
	"#1071", "#1056", "#1057", "#1058", "#1059", "#1046", "#1042",
	"#1068", "#1067", "#1047", "#1064", "#1069", "#1065", "#1063", 
	"#1066"
};

static entity_table_t ent_cp_1251[] = {
	"#1026", "#1027", "#8218", "#1107", "#8222", "hellip", "dagger",
	"Dagger", "euro", "permil", "#1033", "#8249", "#1034", "#1036",
	"#1035", "#1039", "#1106", "#8216", "#8217", "#8219", "#8220",
	"bull", "ndash", "mdash", NULL, "trade", "#1113", "#8250",
	"#1114", "#1116", "#1115", "#1119", "nbsp", "#1038", "#1118",
	"#1032", "curren", "#1168", "brvbar", "sect", "#1025", "copy",
	"#1028", "laquo", "not", "shy", "reg", "#1031", "deg", "plusmn",
	"#1030", "#1110", "#1169", "micro", "para", "middot", "#1105",
	"#8470", "#1108", "raquo", "#1112", "#1029", "#1109", "#1111",
	"#1040", "#1041", "#1042", "#1043", "#1044", "#1045", "#1046",
	"#1047", "#1048", "#1049", "#1050", "#1051", "#1052", "#1053",
	"#1054", "#1055", "#1056", "#1057", "#1058", "#1059", "#1060",
	"#1061", "#1062", "#1063", "#1064", "#1065", "#1066", "#1067",
	"#1068", "#1069", "#1070", "#1071", "#1072", "#1073", "#1074",
	"#1075", "#1076", "#1077", "#1078", "#1079", "#1080", "#1081",
	"#1082", "#1083", "#1084", "#1085", "#1086", "#1087", "#1088",
	"#1089", "#1090", "#1091", "#1092", "#1093", "#1094", "#1095",
	"#1096", "#1097", "#1098", "#1099", "#1100", "#1101", "#1102",
	"#1103"
};

static entity_table_t ent_iso_8859_5[] = {
	"#1056", "#1057", "#1058", "#1059", "#1060", "#1061", "#1062",
	"#1063", "#1064", "#1065", "#1066", "#1067", "#1068", "#1069",
	"#1070", "#1071", "#1072", "#1073", "#1074", "#1075", "#1076",
	"#1077", "#1078", "#1079", "#1080", "#1081", "#1082", "#1083",
	"#1084", "#1085", "#1086", "#1087", "#1088", "#1089", "#1090",
	"#1091", "#1092", "#1093", "#1094", "#1095", "#1096", "#1097",
	"#1098", "#1099", "#1100", "#1101", "#1102", "#1103", "#1104",
	"#1105", "#1106", "#1107", "#1108", "#1109", "#1110", "#1111",
	"#1112", "#1113", "#1114", "#1115", "#1116", "#1117", "#1118",
	"#1119"
};

static entity_table_t ent_cp_866[] = {

	"#9492", "#9524", "#9516", "#9500", "#9472", "#9532", "#9566", 
	"#9567", "#9562", "#9556", "#9577", "#9574", "#9568", "#9552", 
	"#9580", "#9575", "#9576", "#9572", "#9573", "#9561", "#9560", 
	"#9554", "#9555", "#9579", "#9578", "#9496", "#9484", "#9608", 
	"#9604", "#9612", "#9616", "#9600", "#1088", "#1089", "#1090", 
	"#1091", "#1092", "#1093", "#1094", "#1095", "#1096", "#1097", 
	"#1098", "#1099", "#1100", "#1101", "#1102", "#1103", "#1025", 
	"#1105", "#1028", "#1108", "#1031", "#1111", "#1038", "#1118", 
	"#176", "#8729", "#183", "#8730", "#8470", "#164",  "#9632", 
	"#160"
};

/* MacRoman has a couple of low-ascii chars that need mapping too */
/* Vertical tab (ASCII 11) is often used to store line breaks inside */
/* DB exports, this mapping changes it to a space */
static entity_table_t ent_macroman[] = {
	"sp", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, "quot", NULL,
	NULL, NULL, "amp", NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, "lt", NULL, "gt", NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, "Auml", "Aring", "Ccedil", "Eacute", "Ntilde", "Ouml",
	"Uuml", "aacute", "agrave", "acirc", "auml", "atilde", "aring",
	"ccedil", "eacute", "egrave", "ecirc", "euml", "iacute", "igrave",
	"icirc", "iuml", "ntilde", "oacute", "ograve", "ocirc", "ouml",
	"otilde", "uacute", "ugrave", "ucirc", "uuml", "dagger", "deg",
	"cent", "pound", "sect", "bull", "para", "szlig", "reg",
	"copy", "trade", "acute", "uml", "ne", "AElig", "Oslash",
	"infin", "plusmn", "le", "ge", "yen", "micro", "part",
	"sum", "prod", "pi", "int", "ordf", "ordm", "Omega",
	"aelig", "oslash", "iquest", "iexcl", "not", "radic", "fnof",
	"asymp", "#8710", "laquo", "raquo", "hellip", "nbsp", "Agrave",
	"Atilde", "Otilde", "OElig", "oelig", "ndash", "mdash", "ldquo",
	"rdquo", "lsquo", "rsquo", "divide", "loz", "yuml", "Yuml",
	"frasl", "euro", "lsaquo", "rsaquo", "#xFB01", "#xFB02", "Dagger",
	"middot", "sbquo", "bdquo", "permil", "Acirc", "Ecirc", "Aacute",
	"Euml", "Egrave", "Iacute", "Icirc", "Iuml", "Igrave", "Oacute",
	"Ocirc", "#xF8FF", "Ograve", "Uacute", "Ucirc", "Ugrave", "#305",
	"circ", "tilde", "macr", "#728", "#729", "#730", "cedil",
	"#733", "#731", "#711"
};

struct html_entity_map {
	enum entity_charset charset;	/* charset identifier */
	unsigned short basechar;			/* char code at start of table */
	unsigned short endchar;			/* last char code in the table */
	entity_table_t *table;			/* the table of mappings */
};

static const struct html_entity_map entity_map[] = {
	{ cs_cp1252, 		0x80, 0x9f, ent_cp_1252 },
	{ cs_cp1252, 		0xa0, 0xff, ent_iso_8859_1 },
	{ cs_8859_1, 		0xa0, 0xff, ent_iso_8859_1 },
	{ cs_8859_15, 		0xa0, 0xff, ent_iso_8859_15 },
	{ cs_utf_8, 		0xa0, 0xff, ent_iso_8859_1 },
	{ cs_utf_8, 		338,  402,  ent_uni_338_402 },
	{ cs_utf_8, 		710,  732,  ent_uni_spacing },
	{ cs_utf_8, 		913,  982,  ent_uni_greek },
	{ cs_utf_8, 		8194, 8260, ent_uni_punct },
	{ cs_utf_8, 		8364, 8364, ent_uni_euro }, 
	{ cs_utf_8, 		8465, 8501, ent_uni_8465_8501 },
	{ cs_utf_8, 		8592, 9002, ent_uni_8592_9002 },
	{ cs_utf_8, 		9674, 9674, ent_uni_9674 },
	{ cs_utf_8, 		9824, 9830, ent_uni_9824_9830 },
	{ cs_big5, 			0xa0, 0xff, ent_iso_8859_1 },
	{ cs_gb2312, 		0xa0, 0xff, ent_iso_8859_1 },
	{ cs_big5hkscs, 	0xa0, 0xff, ent_iso_8859_1 },
 	{ cs_sjis,			0xa0, 0xff, ent_iso_8859_1 },
 	{ cs_eucjp,			0xa0, 0xff, ent_iso_8859_1 },
	{ cs_koi8r,		    0xa3, 0xff, ent_koi8r },
	{ cs_cp1251,		0x80, 0xff, ent_cp_1251 },
	{ cs_8859_5,		0xc0, 0xff, ent_iso_8859_5 },
	{ cs_cp866,		    0xc0, 0xff, ent_cp_866 },
	{ cs_macroman,		0x0b, 0xff, ent_macroman },
	{ cs_terminator }
};

static const struct {
	const char *codeset;
	enum entity_charset charset;
} charset_map[] = {
	{ "ISO-8859-1", 	cs_8859_1 },
	{ "ISO8859-1",	 	cs_8859_1 },
	{ "ISO-8859-15", 	cs_8859_15 },
	{ "ISO8859-15", 	cs_8859_15 },
	{ "utf-8", 			cs_utf_8 },
	{ "cp1252", 		cs_cp1252 },
	{ "Windows-1252", 	cs_cp1252 },
	{ "1252",           cs_cp1252 }, 
	{ "BIG5",			cs_big5 },
	{ "950",            cs_big5 },
	{ "GB2312",			cs_gb2312 },
	{ "936",            cs_gb2312 },
	{ "BIG5-HKSCS",		cs_big5hkscs },
	{ "Shift_JIS",		cs_sjis },
	{ "SJIS",   		cs_sjis },
	{ "932",            cs_sjis },
	{ "EUCJP",   		cs_eucjp },
	{ "EUC-JP",   		cs_eucjp },
	{ "KOI8-R",         cs_koi8r },
	{ "koi8-ru",        cs_koi8r },
	{ "koi8r",          cs_koi8r },
	{ "cp1251",         cs_cp1251 },
	{ "Windows-1251",   cs_cp1251 },
	{ "win-1251",       cs_cp1251 },
	{ "iso8859-5",      cs_8859_5 },
	{ "iso-8859-5",     cs_8859_5 },
	{ "cp866",          cs_cp866 },
	{ "866",            cs_cp866 },    
	{ "ibm866",         cs_cp866 },
	{ "MacRoman",       cs_macroman },
	{ NULL }
};

static const struct {
	unsigned short charcode;
	char *entity;
	int entitylen;
	int flags;
} basic_entities[] = {
	{ '"',	"&quot;",	6,	ENT_HTML_QUOTE_DOUBLE },
	{ '\'',	"&#039;",	6,	ENT_HTML_QUOTE_SINGLE },
	{ '\'',	"&#39;",	5,	ENT_HTML_QUOTE_SINGLE },
	{ '<',	"&lt;",		4,	0 },
	{ '>',	"&gt;",		4,	0 },
	{ 0, NULL, 0, 0 }
};
	
struct basic_entities_dec {
	unsigned short charcode;
	char entity[8];
	int entitylen;	
};
	
#define MB_RETURN { \
			*newpos = pos;       \
		  	mbseq[mbpos] = '\0'; \
		  	*mbseqlen = mbpos;   \
		  	return this_char; }
					
#define MB_WRITE(mbchar) { \
			mbspace--;  \
			if (mbspace == 0) {      \
				MB_RETURN;           \
			}                        \
			mbseq[mbpos++] = (mbchar); }

#define CHECK_LEN(pos, chars_need)			\
	if((str_len - (pos)) < chars_need) {	\
		*status = FAILURE;					\
		return 0;							\
	}

/* {{{ get_next_char
 */
inline static unsigned short get_next_char(enum entity_charset charset,
		unsigned char * str,
		int str_len,
		int * newpos,
		unsigned char * mbseq,
		int * mbseqlen, 
		int *status)
{
	int pos = *newpos;
	int mbpos = 0;
	int mbspace = *mbseqlen;
	unsigned short this_char = str[pos++];
	unsigned char next_char;

	*status = SUCCESS;
	
	if (mbspace <= 0) {
		*mbseqlen = 0;
		return this_char;
	}
	
	MB_WRITE((unsigned char)this_char);
	
	switch (charset) {
		case cs_utf_8:
			{
				unsigned long utf = 0;
				int stat = 0;
				int more = 1;

				/* unpack utf-8 encoding into a wide char.
				 * Code stolen from the mbstring extension */

				do {
					if (this_char < 0x80) {
						more = 0;
						if(stat) {
							/* we didn't finish the UTF sequence correctly */
							*status = FAILURE;
						}
						break;
					} else if (this_char < 0xc0) {
						switch (stat) {
							case 0x10:	/* 2, 2nd */
							case 0x21:	/* 3, 3rd */
							case 0x32:	/* 4, 4th */
							case 0x43:	/* 5, 5th */
							case 0x54:	/* 6, 6th */
								/* last byte in sequence */
								more = 0;
								utf |= (this_char & 0x3f);
								this_char = (unsigned short)utf;
								break;
							case 0x20:	/* 3, 2nd */
							case 0x31:	/* 4, 3rd */
							case 0x42:	/* 5, 4th */
							case 0x53:	/* 6, 5th */
								/* penultimate char */
								utf |= ((this_char & 0x3f) << 6);
								stat++;
								break;
							case 0x30:	/* 4, 2nd */
							case 0x41:	/* 5, 3rd */
							case 0x52:	/* 6, 4th */
								utf |= ((this_char & 0x3f) << 12);
								stat++;
								break;
							case 0x40:	/* 5, 2nd */
							case 0x51:
								utf |= ((this_char & 0x3f) << 18);
								stat++;
								break;
							case 0x50:	/* 6, 2nd */
								utf |= ((this_char & 0x3f) << 24);
								stat++;
								break;
							default:
								/* invalid */
								*status = FAILURE;
								more = 0;
						}
					}
					/* lead byte */
					else if (this_char < 0xe0) {
						stat = 0x10;	/* 2 byte */
						utf = (this_char & 0x1f) << 6;
						CHECK_LEN(pos, 1);
					} else if (this_char < 0xf0) {
						stat = 0x20;	/* 3 byte */
						utf = (this_char & 0xf) << 12;
						CHECK_LEN(pos, 2);
					} else if (this_char < 0xf8) {
						stat = 0x30;	/* 4 byte */
						utf = (this_char & 0x7) << 18;
						CHECK_LEN(pos, 3);
					} else if (this_char < 0xfc) {
						stat = 0x40;	/* 5 byte */
						utf = (this_char & 0x3) << 24;
						CHECK_LEN(pos, 4);
					} else if (this_char < 0xfe) {
						stat = 0x50;	/* 6 byte */
						utf = (this_char & 0x1) << 30;
						CHECK_LEN(pos, 5);
					} else {
						/* invalid; bail */
						more = 0;
						*status = FAILURE;
						break;
					}

					if (more) {
						this_char = str[pos++];
						MB_WRITE((unsigned char)this_char);
					}
				} while (more);
			}
			break;
		case cs_big5:
		case cs_gb2312:
		case cs_big5hkscs:
			{
				/* check if this is the first of a 2-byte sequence */
				if (this_char >= 0xa1 && this_char <= 0xfe) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos];
					if ((next_char >= 0x40 && next_char <= 0x7e) ||
							(next_char >= 0xa1 && next_char <= 0xfe)) {
						/* yes, this a wide char */
						this_char <<= 8;
						MB_WRITE(next_char);
						this_char |= next_char;
						pos++;
					}
					
				}
				break;
			}
		case cs_sjis:
			{
				/* check if this is the first of a 2-byte sequence */
				if ( (this_char >= 0x81 && this_char <= 0x9f) ||
					 (this_char >= 0xe0 && this_char <= 0xef)
					) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos];
					if ((next_char >= 0x40 && next_char <= 0x7e) ||
						(next_char >= 0x80 && next_char <= 0xfc))
					{
						/* yes, this a wide char */
						this_char <<= 8;
						MB_WRITE(next_char);
						this_char |= next_char;
						pos++;
					}
					
				}
				break;
			}
		case cs_eucjp:
			{
				/* check if this is the first of a multi-byte sequence */
				if (this_char >= 0xa1 && this_char <= 0xfe) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos];
					if (next_char >= 0xa1 && next_char <= 0xfe) {
						/* yes, this a jis kanji char */
						this_char <<= 8;
						MB_WRITE(next_char);
						this_char |= next_char;
						pos++;
					}
					
				} else if (this_char == 0x8e) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos];
					if (next_char >= 0xa1 && next_char <= 0xdf) {
						/* JIS X 0201 kana */
						this_char <<= 8;
						MB_WRITE(next_char);
						this_char |= next_char;
						pos++;
					}
					
				} else if (this_char == 0x8f) {
					/* peek at the next two char */
					unsigned char next2_char;
					CHECK_LEN(pos, 2);
					next_char = str[pos];
					next2_char = str[pos+1];
					if ((next_char >= 0xa1 && next_char <= 0xfe) &&
						(next2_char >= 0xa1 && next2_char <= 0xfe)) {
						/* JIS X 0212 hojo-kanji */
						this_char <<= 8;
						MB_WRITE(next_char);
						this_char |= next_char;
						pos++;
						this_char <<= 8;
						MB_WRITE(next2_char);
						this_char |= next2_char;
						pos++;
					}
					
				}
				break;
			}
		default:
			break;
	}
	MB_RETURN;
}
/* }}} */

/* {{{ entity_charset determine_charset
 * returns the charset identifier based on current locale or a hint.
 * defaults to iso-8859-1 */
static enum entity_charset determine_charset(char *charset_hint TSRMLS_DC)
{
	int i;
	enum entity_charset charset = cs_8859_1;
	int len = 0;
	zval *uf_result = NULL;

	/* Guarantee default behaviour for backwards compatibility */
	if (charset_hint == NULL)
		return cs_8859_1;

	if ((len = strlen(charset_hint)) != 0) {
		goto det_charset;
	}
#if HAVE_MBSTRING
#if !defined(COMPILE_DL_MBSTRING)
	/* XXX: Ugly things. Why don't we look for a more sophisticated way? */
	switch (MBSTRG(current_internal_encoding)) {
		case mbfl_no_encoding_8859_1:
			return cs_8859_1;

		case mbfl_no_encoding_utf8:
			return cs_utf_8;

		case mbfl_no_encoding_euc_jp:
		case mbfl_no_encoding_eucjp_win:
			return cs_eucjp;

		case mbfl_no_encoding_sjis:
		case mbfl_no_encoding_sjis_win:
		case mbfl_no_encoding_sjis_mac:
			return cs_sjis;

		case mbfl_no_encoding_cp1252:
			return cs_cp1252;

		case mbfl_no_encoding_8859_15:
			return cs_8859_15;

		case mbfl_no_encoding_big5:
			return cs_big5;

		case mbfl_no_encoding_euc_cn:
		case mbfl_no_encoding_hz:
		case mbfl_no_encoding_cp936:
			return cs_gb2312;

		case mbfl_no_encoding_koi8r:
			return cs_koi8r;

		case mbfl_no_encoding_cp866:
			return cs_cp866;

		case mbfl_no_encoding_cp1251:
			return cs_cp1251;

		case mbfl_no_encoding_8859_5:
			return cs_8859_5;

		default:
			;
	}
#else
	{
		zval nm_mb_internal_encoding;

		ZVAL_STRING(&nm_mb_internal_encoding, "mb_internal_encoding", 0);

		if (call_user_function_ex(CG(function_table), NULL, &nm_mb_internal_encoding, &uf_result, 0, NULL, 1, NULL TSRMLS_CC) != FAILURE) {

			charset_hint = Z_STRVAL_P(uf_result);
			len = Z_STRLEN_P(uf_result);
			
			if (len == 4) { /* sizeof(none|auto|pass)-1 */
				if (!memcmp("pass", charset_hint, sizeof("pass") - 1) || 
				    !memcmp("auto", charset_hint, sizeof("auto") - 1) || 
				    !memcmp("none", charset_hint, sizeof("none") - 1)) {
					
					charset_hint = NULL;
					len = 0;
				}
			}
			goto det_charset;
		}
	}
#endif
#endif

	charset_hint = SG(default_charset);
	if (charset_hint != NULL && (len=strlen(charset_hint)) != 0) {
		goto det_charset;
	}

	/* try to detect the charset for the locale */
#if HAVE_NL_LANGINFO && HAVE_LOCALE_H && defined(CODESET)
	charset_hint = nl_langinfo(CODESET);
	if (charset_hint != NULL && (len=strlen(charset_hint)) != 0) {
		goto det_charset;
	}
#endif

#if HAVE_LOCALE_H
	/* try to figure out the charset from the locale */
	{
		char *localename;
		char *dot, *at;

		/* lang[_territory][.codeset][@modifier] */
		localename = setlocale(LC_CTYPE, NULL);

		dot = strchr(localename, '.');
		if (dot) {
			dot++;
			/* locale specifies a codeset */
			at = strchr(dot, '@');
			if (at)
				len = at - dot;
			else
				len = strlen(dot);
			charset_hint = dot;
		} else {
			/* no explicit name; see if the name itself
			 * is the charset */
			charset_hint = localename;
			len = strlen(charset_hint);
		}
	}
#endif

det_charset:

	if (charset_hint) {
		int found = 0;
		
		/* now walk the charset map and look for the codeset */
		for (i = 0; charset_map[i].codeset; i++) {
			if (len == strlen(charset_map[i].codeset) && strncasecmp(charset_hint, charset_map[i].codeset, len) == 0) {
				charset = charset_map[i].charset;
				found = 1;
				break;
			}
		}
		if (!found) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "charset `%s' not supported, assuming iso-8859-1",
					charset_hint);
		}
	}
	if (uf_result != NULL) {
		zval_ptr_dtor(&uf_result);
	}
	return charset;
}
/* }}} */

/* {{{ php_utf32_utf8 */
size_t php_utf32_utf8(unsigned char *buf, int k)
{
	size_t retval = 0;

	if (k < 0x80) {
		buf[0] = k;
		retval = 1;
	} else if (k < 0x800) {
		buf[0] = 0xc0 | (k >> 6);
		buf[1] = 0x80 | (k & 0x3f);
		retval = 2;
	} else if (k < 0x10000) {
		buf[0] = 0xe0 | (k >> 12);
		buf[1] = 0x80 | ((k >> 6) & 0x3f);
		buf[2] = 0x80 | (k & 0x3f);
		retval = 3;
	} else if (k < 0x200000) {
		buf[0] = 0xf0 | (k >> 18);
		buf[1] = 0x80 | ((k >> 12) & 0x3f);
		buf[2] = 0x80 | ((k >> 6) & 0x3f);
		buf[3] = 0x80 | (k & 0x3f);
		retval = 4;
	} else if (k < 0x4000000) {
		buf[0] = 0xf8 | (k >> 24);
		buf[1] = 0x80 | ((k >> 18) & 0x3f);
		buf[2] = 0x80 | ((k >> 12) & 0x3f);
		buf[3] = 0x80 | ((k >> 6) & 0x3f);
		buf[4] = 0x80 | (k & 0x3f);
		retval = 5;
	} else {
		buf[0] = 0xfc | (k >> 30);
		buf[1] = 0x80 | ((k >> 24) & 0x3f);
		buf[2] = 0x80 | ((k >> 18) & 0x3f);
		buf[3] = 0x80 | ((k >> 12) & 0x3f);
		buf[4] = 0x80 | ((k >> 6) & 0x3f);
		buf[5] = 0x80 | (k & 0x3f);
		retval = 6;
	}
	buf[retval] = '\0';

	return retval;
}
/* }}} */

/* {{{ php_unescape_html_entities
 */
PHPAPI char *php_unescape_html_entities(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset TSRMLS_DC)
{
	int retlen;
	int j, k;
	char *replaced, *ret, *p, *q, *lim, *next;
	enum entity_charset charset = determine_charset(hint_charset TSRMLS_CC);
	unsigned char replacement[15];
	int replacement_len;

	ret = estrndup(old, oldlen);
	retlen = oldlen;
	if (!retlen) {
		goto empty_source;
	}
	
	if (all) {
		/* look for a match in the maps for this charset */
		for (j = 0; entity_map[j].charset != cs_terminator; j++) {
			if (entity_map[j].charset != charset)
				continue;

			for (k = entity_map[j].basechar; k <= entity_map[j].endchar; k++) {
				unsigned char entity[32];
				int entity_length = 0;

				if (entity_map[j].table[k - entity_map[j].basechar] == NULL)
					continue;

				entity_length = slprintf(entity, sizeof(entity), "&%s;", entity_map[j].table[k - entity_map[j].basechar]);
				if (entity_length >= sizeof(entity)) {
					continue;
				}

				/* When we have MBCS entities in the tables above, this will need to handle it */
				replacement_len = 0;
				switch (charset) {
					case cs_8859_1:
					case cs_cp1252:
					case cs_8859_15:
					case cs_cp1251:
					case cs_8859_5:
					case cs_cp866:
					case cs_koi8r:
						replacement[0] = k;
						replacement[1] = '\0';
						replacement_len = 1;
						break;

					case cs_big5:
					case cs_gb2312:
					case cs_big5hkscs:
					case cs_sjis:
					case cs_eucjp:
						/* we cannot properly handle those multibyte encodings
						 * with php_str_to_str. skip it. */ 
						continue;

					case cs_utf_8:
						replacement_len = php_utf32_utf8(replacement, k);
						break;

					default:
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot yet handle MBCS!");
						efree(ret);
						return NULL;
				}

				if (php_memnstr(ret, entity, entity_length, ret+retlen)) {
					replaced = php_str_to_str(ret, retlen, entity, entity_length, replacement, replacement_len, &retlen);
					efree(ret);
					ret = replaced;
				}
			}
		}
	}

	for (j = 0; basic_entities[j].charcode != 0; j++) {

		if (basic_entities[j].flags && (quote_style & basic_entities[j].flags) == 0)
			continue;
		
		replacement[0] = (unsigned char)basic_entities[j].charcode;
		replacement[1] = '\0';

		if (php_memnstr(ret, basic_entities[j].entity, basic_entities[j].entitylen, ret+retlen)) {		
			replaced = php_str_to_str(ret, retlen, basic_entities[j].entity, basic_entities[j].entitylen, replacement, 1, &retlen);
			efree(ret);
			ret = replaced;
		}
	}

	/* replace numeric entities & "&amp;" */
	lim = ret + retlen;
	for (p = ret, q = ret; p < lim;) {
		int code;

		if (p[0] == '&') {
			if (p + 2 < lim) {
				if (p[1] == '#') {
					int invalid_code = 0;

					if (p[2] == 'x' || p[2] == 'X') {
						code = strtol(p + 3, &next, 16);
					} else {
						code = strtol(p + 2, &next, 10);
					}

					if (next != NULL && *next == ';') {
						switch (charset) {
							case cs_utf_8:
								q += php_utf32_utf8(q, code);
								break;

							case cs_8859_1:
							case cs_8859_5:
							case cs_8859_15:
								if ((code >= 0x80 && code < 0xa0) || code > 0xff) {
									invalid_code = 1;
								} else {
									if (code == 39 || !quote_style) {
										invalid_code = 1;
									} else {
										*(q++) = code;
									}
								}
								break;

							case cs_cp1252:
							case cs_cp1251:
							case cs_cp866:
								if (code > 0xff) {
									invalid_code = 1;
								} else {
									*(q++) = code;
								}
								break;

							case cs_big5:
							case cs_big5hkscs:
							case cs_sjis:
							case cs_eucjp:
								if (code >= 0x80) {
									invalid_code = 1;
								} else {
									*(q++) = code;
								}
								break;

							case cs_gb2312:
								if (code >= 0x81) {
									invalid_code = 1;
								} else {
									*(q++) = code;
								}
								break;

							default:
								/* for backwards compatilibity */
								invalid_code = 1;
								break;
						}
						if (invalid_code) {
							for (; p <= next; p++) {
								*(q++) = *p;
							}
						}
						p = next + 1;
					} else {
						*(q++) = *(p++);	
						*(q++) = *(p++);	
					}
				} else if (p + 4 < lim &&
							p[1] == 'a' && p[2] == 'm' &&p[3] == 'p' &&
							p[4] == ';') {
					*(q++) = '&';
					p += 5;
				} else {
					*(q++) = *(p++);
					*(q++) = *(p++);
				}
			} else {
				*(q++) = *(p++);	
			}
		} else {
			*(q++) = *(p++);	
		}
	}
	*q = '\0';
	retlen = (size_t)(q - ret);
empty_source:	
	*newlen = retlen;
	return ret;
}
/* }}} */

PHPAPI char *php_escape_html_entities(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset TSRMLS_DC)
{
	return php_escape_html_entities_ex(old, oldlen, newlen, all, quote_style, hint_charset, 1 TSRMLS_CC);
}


/* {{{ php_escape_html_entities
 */
PHPAPI char *php_escape_html_entities_ex(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset, zend_bool double_encode TSRMLS_DC)
{
	int i, j, maxlen, len;
	char *replaced;
	enum entity_charset charset = determine_charset(hint_charset TSRMLS_CC);
	int matches_map;

	maxlen = 2 * oldlen;
	if (maxlen < 128)
		maxlen = 128;
	replaced = emalloc (maxlen);
	len = 0;
	i = 0;
	while (i < oldlen) {
		unsigned char mbsequence[16];	/* allow up to 15 characters in a multibyte sequence */
		int mbseqlen = sizeof(mbsequence);
		int status = SUCCESS;
		unsigned short this_char = get_next_char(charset, old, oldlen, &i, mbsequence, &mbseqlen, &status);

		if(status == FAILURE) {
			/* invalid MB sequence */
			efree(replaced);
			if(!PG(display_errors)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid multibyte sequence in argument");
			}
			*newlen = 0;
			return STR_EMPTY_ALLOC();
		}
		matches_map = 0;

		if (len + 16 > maxlen)
			replaced = erealloc (replaced, maxlen += 128);

		if (all) {
			/* look for a match in the maps for this charset */
			unsigned char *rep = NULL;


			for (j = 0; entity_map[j].charset != cs_terminator; j++) {
				if (entity_map[j].charset == charset
						&& this_char >= entity_map[j].basechar
						&& this_char <= entity_map[j].endchar) {
					rep = (unsigned char*)entity_map[j].table[this_char - entity_map[j].basechar];
					if (rep == NULL) {
						/* there is no entity for this position; fall through and
						 * just output the character itself */
						break;
					}

					matches_map = 1;
					break;
				}
			}

			if (matches_map) {
				int l = strlen(rep);
				/* increase the buffer size */
				if (len + 2 + l >= maxlen) {
					replaced = erealloc(replaced, maxlen += 128);
				}

				replaced[len++] = '&';
				strlcpy(replaced + len, rep, maxlen);
				len += l;
				replaced[len++] = ';';
			}
		}
		if (!matches_map) {	
			int is_basic = 0;

			if (this_char == '&') {
				if (double_encode) {
encode_amp:
					memcpy(replaced + len, "&amp;", sizeof("&amp;") - 1);
					len += sizeof("&amp;") - 1;
				} else {
					char *e = memchr(old + i, ';', oldlen - i);
					char *s = old + i;

					if (!e || (e - s) > 10) { /* minor optimization to avoid "entities" over 10 chars in length */
						goto encode_amp;
					} else {
						if (*s == '#') { /* numeric entities */
							s++;
							/* Hex (&#x5A;) */
							if (*s == 'x' || *s == 'X') {
								s++;
								while (s < e) {
									if (!isxdigit((int)*(unsigned char *)s++)) {
										goto encode_amp;
									}
								}
							/* Dec (&#90;)*/
							} else {
								while (s < e) {
									if (!isdigit((int)*(unsigned char *)s++)) {
										goto encode_amp;
									}
								}
							}
						} else { /* text entities */
							while (s < e) {
								if (!isalnum((int)*(unsigned char *)s++)) {
									goto encode_amp;
								}
							}
						}
						replaced[len++] = '&';
					}
				}
				is_basic = 1;
			} else {
				for (j = 0; basic_entities[j].charcode != 0; j++) {
					if ((basic_entities[j].charcode != this_char) ||
							(basic_entities[j].flags &&
							(quote_style & basic_entities[j].flags) == 0)) {
						continue;
					}

					memcpy(replaced + len, basic_entities[j].entity, basic_entities[j].entitylen);
					len += basic_entities[j].entitylen;
		
					is_basic = 1;
					break;
				}
			}

			if (!is_basic) {
				/* a wide char without a named entity; pass through the original sequence */
				if (mbseqlen > 1) {
					memcpy(replaced + len, mbsequence, mbseqlen);
					len += mbseqlen;
				} else {
					replaced[len++] = (unsigned char)this_char;
				}
			}
		}
	}
	replaced[len] = '\0';
	*newlen = len;

	return replaced;


}
/* }}} */

/* {{{ php_html_entities
 */
static void php_html_entities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
	char *str, *hint_charset = NULL;
	int str_len, hint_charset_len = 0;
	int len;
	long quote_style = ENT_COMPAT;
	char *replaced;
	zend_bool double_encode = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!b", &str, &str_len, &quote_style, &hint_charset, &hint_charset_len, &double_encode) == FAILURE) {
		return;
	}

	replaced = php_escape_html_entities_ex(str, str_len, &len, all, quote_style, hint_charset, double_encode TSRMLS_CC);
	RETVAL_STRINGL(replaced, len, 0);
}
/* }}} */

#define HTML_SPECIALCHARS 	0
#define HTML_ENTITIES	 	1

/* {{{ register_html_constants
 */
void register_html_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("HTML_SPECIALCHARS", HTML_SPECIALCHARS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("HTML_ENTITIES", HTML_ENTITIES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_COMPAT", ENT_COMPAT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_QUOTES", ENT_QUOTES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_NOQUOTES", ENT_NOQUOTES, CONST_PERSISTENT|CONST_CS);
}
/* }}} */

/* {{{ proto string htmlspecialchars(string string [, int quote_style[, string charset[, bool double_encode]]])
   Convert special characters to HTML entities */
PHP_FUNCTION(htmlspecialchars)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string htmlspecialchars_decode(string string [, int quote_style])
   Convert special HTML entities back to characters */
PHP_FUNCTION(htmlspecialchars_decode)
{
	char *str, *new_str, *e, *p;
	int len, j, i, new_len;
	long quote_style = ENT_COMPAT;
	struct basic_entities_dec basic_entities_dec[8];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &len, &quote_style) == FAILURE) {
		return;
	}

	new_str = estrndup(str, len);
	new_len = len;
	e = new_str + new_len;

	if (!(p = memchr(new_str, '&', new_len))) {
		RETURN_STRINGL(new_str, new_len, 0);
	}

	for (j = 0, i = 0; basic_entities[i].charcode != 0; i++) {
		if (basic_entities[i].flags && !(quote_style & basic_entities[i].flags)) {
			continue;
		}
		basic_entities_dec[j].charcode = basic_entities[i].charcode;
		memcpy(basic_entities_dec[j].entity, basic_entities[i].entity, basic_entities[i].entitylen + 1);
		basic_entities_dec[j].entitylen = basic_entities[i].entitylen;
		j++;
	}
	basic_entities_dec[j].charcode = '&';
	basic_entities_dec[j].entitylen = sizeof("&amp;") - 1;
	memcpy(basic_entities_dec[j].entity, "&amp;", sizeof("&amp;"));
	i = j + 1;
	
	do {
		int l = e - p;
	
		for (j = 0; j < i; j++) {
			if (basic_entities_dec[j].entitylen > l) {
				continue;
			}
			if (!memcmp(p, basic_entities_dec[j].entity, basic_entities_dec[j].entitylen)) {
				int e_len = basic_entities_dec[j].entitylen - 1;
		
				*p++ = basic_entities_dec[j].charcode;
				memmove(p, p + e_len, (e - p - e_len));
				e -= e_len;
				goto done;
			}
		}
		p++;

done:
		if (p >= e) {
			break;
		}
	} while ((p = memchr(p, '&', (e - p))));

	new_len = e - new_str;

	new_str[new_len] = '\0';
	RETURN_STRINGL(new_str, new_len, 0);
}
/* }}} */

/* {{{ proto string html_entity_decode(string string [, int quote_style][, string charset])
   Convert all HTML entities to their applicable characters */
PHP_FUNCTION(html_entity_decode)
{
	char *str, *hint_charset = NULL;
	int str_len, hint_charset_len, len;
	long quote_style = ENT_COMPAT;
	char *replaced;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len,
							  &quote_style, &hint_charset, &hint_charset_len) == FAILURE) {
		return;
	}

	replaced = php_unescape_html_entities(str, str_len, &len, 1, quote_style, hint_charset TSRMLS_CC);
	if (replaced) {
		RETURN_STRINGL(replaced, len, 0);
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto string htmlentities(string string [, int quote_style[, string charset[, bool double_encode]]])
   Convert all applicable characters to HTML entities */
PHP_FUNCTION(htmlentities)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array get_html_translation_table([int table [, int quote_style]])
   Returns the internal translation table used by htmlspecialchars and htmlentities */
PHP_FUNCTION(get_html_translation_table)
{
	long which = HTML_SPECIALCHARS, quote_style = ENT_COMPAT;
	int i, j;
	char ind[2];
	enum entity_charset charset = determine_charset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &which, &quote_style) == FAILURE) {
		return;
	}

	array_init(return_value);

	ind[1] = 0;

	switch (which) {
		case HTML_ENTITIES:
			for (j=0; entity_map[j].charset != cs_terminator; j++) {
				if (entity_map[j].charset != charset)
					continue;
				for (i = 0; i <= entity_map[j].endchar - entity_map[j].basechar; i++) {
					char buffer[16];

					if (entity_map[j].table[i] == NULL)
						continue;
					/* what about wide chars here ?? */
					ind[0] = i + entity_map[j].basechar;
					snprintf(buffer, sizeof(buffer), "&%s;", entity_map[j].table[i]);
					add_assoc_string(return_value, ind, buffer, 1);

				}
			}
			/* break thru */

		case HTML_SPECIALCHARS:
			for (j = 0; basic_entities[j].charcode != 0; j++) {

				if (basic_entities[j].flags && (quote_style & basic_entities[j].flags) == 0)
					continue;
				
				ind[0] = (unsigned char)basic_entities[j].charcode;
				add_assoc_stringl(return_value, ind, basic_entities[j].entity, basic_entities[j].entitylen, 1);
			}
			add_assoc_stringl(return_value, "&", "&amp;", sizeof("&amp;") - 1, 1);

			break;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
