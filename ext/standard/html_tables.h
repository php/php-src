/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id: html.h 293036 2010-01-03 09:23:27Z sebastian $ */

#ifndef HTML_TABLES_H
#define HTML_TABLES_H

/* cs_terminator is overloaded in the following fashion:
 * - It terminates the list entity maps.
 * - In BG(inverse_ent_maps), it's the key of the inverse map that stores
 *   only the basic entities.
 * - When passed to traverse_for_entities (or via php_unescape_entities with !all),
 *   we don't care about the encoding (UTF-8 is chosen, but it should be used
 *   when it doesn't matter).
 */
enum entity_charset { cs_terminator, cs_8859_1, cs_cp1252,
					  cs_8859_15, cs_utf_8, cs_big5, cs_gb2312, 
					  cs_big5hkscs, cs_sjis, cs_eucjp, cs_koi8r,
					  cs_cp1251, cs_8859_5, cs_cp866, cs_macroman,
					  cs_numelems /* used to count the number of charsets */
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
	"dagger", "Dagger", "bull", NULL, NULL, NULL, "hellip",
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
	"lArr", "uArr", "rArr", "dArr", "hArr", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8672 (0x21e0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8704 (0x2200) */
	"forall", NULL, "part", "exist", NULL, "empty", NULL, "nabla",
	"isin", "notin", NULL, "ni", NULL, NULL, NULL, "prod",
	/* 8720 (0x2210) */
	NULL, "sum", "minus", NULL, NULL, NULL, NULL, "lowast",
	NULL, NULL, "radic", NULL, NULL, "prop", "infin", NULL,
	/* 8736 (0x2220) */
	"ang", NULL, NULL, NULL, NULL, NULL, NULL, "and",
	"or", "cap", "cup", "int", NULL, NULL, NULL, NULL,
	/* 8752 (0x2230) */
	NULL, NULL, NULL, NULL, "there4", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, "sim", NULL, NULL, NULL,
	/* 8768 (0x2240) */
	NULL, NULL, NULL, NULL, NULL, "cong", NULL, NULL,
	"asymp", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8784 (0x2250) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8800 (0x2260) */
	"ne", "equiv", NULL, NULL, "le", "ge", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8816 (0x2270) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8832 (0x2280) */
	NULL, NULL, "sub", "sup", "nsub", NULL, "sube", "supe",
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
	unsigned int basechar;			/* char code at start of table */
	unsigned int endchar;			/* last char code in the table */
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

typedef struct {
	unsigned short charcode;
	char *entity;
	int entitylen;
	int flags;
} basic_entity_t;

static const basic_entity_t basic_entities_ex[] = {
	{ '&',  "&amp;",    5,  0 },
	{ '"',	"&quot;",	6,	ENT_HTML_QUOTE_DOUBLE },
	/* PHP traditionally encodes ' as &#039;, not &apos;, so leave this entry here */
	{ '\'',	"&#039;",	6,	ENT_HTML_QUOTE_SINGLE },
	{ '\'',	"&apos;",	6,	ENT_HTML_QUOTE_SINGLE },
	{ '<',	"&lt;",		4,	0 },
	{ '>',	"&gt;",		4,	0 },
	{ 0, NULL, 0, 0 }
};

/* In some cases, we need to give special treatment to &, so we
 * use this instead */
static const basic_entity_t *basic_entities = &basic_entities_ex[1];

typedef struct {
	unsigned short un_code_point; /* we don't need bigger */
	unsigned char cs_code; /* currently, we only have maps to single-byte encodings */
} unicode_mapping;

static const unicode_mapping unimap_iso885915[] = {
	{ 0xA5,   0xA5 },	/* yen sign */
	{ 0xA7,   0xA7 },	/* section sign */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAA,   0xAA },	/* feminine ordinal indicator */
	{ 0xAB,   0xAB },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xAC },	/* not sign */
	{ 0xAD,   0xAD },	/* soft hyphen */
	{ 0xAE,   0xAE },	/* registered sign */
	{ 0xAF,   0xAF },	/* macron */
	{ 0xB0,   0xB0 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB2,   0xB2 },	/* superscript two */
	{ 0xB3,   0xB3 },	/* superscript three */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xB6 },	/* pilcrow sign */
	{ 0xB7,   0xB7 },	/* middle dot */
	{ 0xB9,   0xB9 },	/* superscript one */
	{ 0xBA,   0xBA },	/* masculine ordinal indicator */
	{ 0xBB,   0xBB },	/* right-pointing double angle quotation mark */
	{ 0x152,  0xBC },	/* latin capital ligature oe */
	{ 0x153,  0xBD },	/* latin small ligature oe */
	{ 0x160,  0xA6 },	/* latin capital letter s with caron */
	{ 0x161,  0xA8 },	/* latin small letter s with caron */
	{ 0x178,  0xBE },	/* latin capital letter y with diaeresis */
	{ 0x17D,  0xB4 },	/* latin capital letter z with caron */
	{ 0x17E,  0xB8 },	/* latin small letter z with caron */
	{ 0x20AC, 0xA4 },	/* euro sign */
};

static const unicode_mapping unimap_win1252[] = {
	{ 0x152,  0x8C },	/* latin capital ligature oe */
	{ 0x153,  0x9C },	/* latin small ligature oe */
	{ 0x160,  0x8A },	/* latin capital letter s with caron */
	{ 0x161,  0x9A },	/* latin small letter s with caron */
	{ 0x178,  0x9F },	/* latin capital letter y with diaeresis */
	{ 0x17D,  0x8E },	/* latin capital letter z with caron */
	{ 0x17E,  0x9E },	/* latin small letter z with caron */
	{ 0x192,  0x83 },	/* latin small letter f with hook */
	{ 0x2C6,  0x88 },	/* modifier letter circumflex accent */
	{ 0x2DC,  0x98 },	/* small tilde */
	{ 0x2013, 0x96 },	/* en dash */
	{ 0x2014, 0x97 },	/* em dash */
	{ 0x2018, 0x91 },	/* left single quotation mark */
	{ 0x2019, 0x92 },	/* right single quotation mark */
	{ 0x201A, 0x82 },	/* single low-9 quotation mark */
	{ 0x201C, 0x93 },	/* left double quotation mark */
	{ 0x201D, 0x94 },	/* right double quotation mark */
	{ 0x201E, 0x84 },	/* double low-9 quotation mark */
	{ 0x2020, 0x86 },	/* dagger */
	{ 0x2021, 0x87 },	/* double dagger */
	{ 0x2022, 0x95 },	/* bullet */
	{ 0x2026, 0x85 },	/* horizontal ellipsis */
	{ 0x2030, 0x89 },	/* per mille sign */
	{ 0x2039, 0x8B },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0x9B },	/* single right-pointing angle quotation mark */
	{ 0x20AC, 0x80 },	/* euro sign */
	{ 0x2122, 0x99 },	/* trade mark sign */
};

static const unicode_mapping unimap_win1251[] = {
	{ 0xA0,   0xA0 },	/* no-break space */
	{ 0xA4,   0xA4 },	/* currency sign */
	{ 0xA6,   0xA6 },	/* broken bar */
	{ 0xA7,   0xA7 },	/* section sign */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAB,   0xAB },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xAC },	/* not sign */
	{ 0xAD,   0xAD },	/* soft hyphen */
	{ 0xAE,   0xAE },	/* registered sign */
	{ 0xB0,   0xB0 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xB6 },	/* pilcrow sign */
	{ 0xB7,   0xB7 },	/* middle dot */
	{ 0xBB,   0xBB },	/* right-pointing double angle quotation mark */
	{ 0x401,  0xA8 },	/* cyrillic capital letter io */
	{ 0x402,  0x80 },	/* cyrillic capital letter dje */
	{ 0x403,  0x81 },	/* cyrillic capital letter gje */
	{ 0x404,  0xAA },	/* cyrillic capital letter ukrainian ie */
	{ 0x405,  0xBD },	/* cyrillic capital letter dze */
	{ 0x406,  0xB2 },	/* cyrillic capital letter byelorussian-ukrainian i */
	{ 0x407,  0xAF },	/* cyrillic capital letter yi */
	{ 0x408,  0xA3 },	/* cyrillic capital letter je */
	{ 0x409,  0x8A },	/* cyrillic capital letter lje */
	{ 0x40A,  0x8C },	/* cyrillic capital letter nje */
	{ 0x40B,  0x8E },	/* cyrillic capital letter tshe */
	{ 0x40C,  0x8D },	/* cyrillic capital letter kje */
	{ 0x40E,  0xA1 },	/* cyrillic capital letter short u */
	{ 0x40F,  0x8F },	/* cyrillic capital letter dzhe */
	{ 0x410,  0xC0 },	/* cyrillic capital letter a */
	{ 0x411,  0xC1 },	/* cyrillic capital letter be */
	{ 0x412,  0xC2 },	/* cyrillic capital letter ve */
	{ 0x413,  0xC3 },	/* cyrillic capital letter ghe */
	{ 0x414,  0xC4 },	/* cyrillic capital letter de */
	{ 0x415,  0xC5 },	/* cyrillic capital letter ie */
	{ 0x416,  0xC6 },	/* cyrillic capital letter zhe */
	{ 0x417,  0xC7 },	/* cyrillic capital letter ze */
	{ 0x418,  0xC8 },	/* cyrillic capital letter i */
	{ 0x419,  0xC9 },	/* cyrillic capital letter short i */
	{ 0x41A,  0xCA },	/* cyrillic capital letter ka */
	{ 0x41B,  0xCB },	/* cyrillic capital letter el */
	{ 0x41C,  0xCC },	/* cyrillic capital letter em */
	{ 0x41D,  0xCD },	/* cyrillic capital letter en */
	{ 0x41E,  0xCE },	/* cyrillic capital letter o */
	{ 0x41F,  0xCF },	/* cyrillic capital letter pe */
	{ 0x420,  0xD0 },	/* cyrillic capital letter er */
	{ 0x421,  0xD1 },	/* cyrillic capital letter es */
	{ 0x422,  0xD2 },	/* cyrillic capital letter te */
	{ 0x423,  0xD3 },	/* cyrillic capital letter u */
	{ 0x424,  0xD4 },	/* cyrillic capital letter ef */
	{ 0x425,  0xD5 },	/* cyrillic capital letter ha */
	{ 0x426,  0xD6 },	/* cyrillic capital letter tse */
	{ 0x427,  0xD7 },	/* cyrillic capital letter che */
	{ 0x428,  0xD8 },	/* cyrillic capital letter sha */
	{ 0x429,  0xD9 },	/* cyrillic capital letter shcha */
	{ 0x42A,  0xDA },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0xDB },	/* cyrillic capital letter yeru */
	{ 0x42C,  0xDC },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0xDD },	/* cyrillic capital letter e */
	{ 0x42E,  0xDE },	/* cyrillic capital letter yu */
	{ 0x42F,  0xDF },	/* cyrillic capital letter ya */
	{ 0x430,  0xE0 },	/* cyrillic small letter a */
	{ 0x431,  0xE1 },	/* cyrillic small letter be */
	{ 0x432,  0xE2 },	/* cyrillic small letter ve */
	{ 0x433,  0xE3 },	/* cyrillic small letter ghe */
	{ 0x434,  0xE4 },	/* cyrillic small letter de */
	{ 0x435,  0xE5 },	/* cyrillic small letter ie */
	{ 0x436,  0xE6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xE7 },	/* cyrillic small letter ze */
	{ 0x438,  0xE8 },	/* cyrillic small letter i */
	{ 0x439,  0xE9 },	/* cyrillic small letter short i */
	{ 0x43A,  0xEA },	/* cyrillic small letter ka */
	{ 0x43B,  0xEB },	/* cyrillic small letter el */
	{ 0x43C,  0xEC },	/* cyrillic small letter em */
	{ 0x43D,  0xED },	/* cyrillic small letter en */
	{ 0x43E,  0xEE },	/* cyrillic small letter o */
	{ 0x43F,  0xEF },	/* cyrillic small letter pe */
	{ 0x440,  0xF0 },	/* cyrillic small letter er */
	{ 0x441,  0xF1 },	/* cyrillic small letter es */
	{ 0x442,  0xF2 },	/* cyrillic small letter te */
	{ 0x443,  0xF3 },	/* cyrillic small letter u */
	{ 0x444,  0xF4 },	/* cyrillic small letter ef */
	{ 0x445,  0xF5 },	/* cyrillic small letter ha */
	{ 0x446,  0xF6 },	/* cyrillic small letter tse */
	{ 0x447,  0xF7 },	/* cyrillic small letter che */
	{ 0x448,  0xF8 },	/* cyrillic small letter sha */
	{ 0x449,  0xF9 },	/* cyrillic small letter shcha */
	{ 0x44A,  0xFA },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xFB },	/* cyrillic small letter yeru */
	{ 0x44C,  0xFC },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xFD },	/* cyrillic small letter e */
	{ 0x44E,  0xFE },	/* cyrillic small letter yu */
	{ 0x44F,  0xFF },	/* cyrillic small letter ya */
	{ 0x451,  0xB8 },	/* cyrillic small letter io */
	{ 0x452,  0x90 },	/* cyrillic small letter dje */
	{ 0x453,  0x83 },	/* cyrillic small letter gje */
	{ 0x454,  0xBA },	/* cyrillic small letter ukrainian ie */
	{ 0x455,  0xBE },	/* cyrillic small letter dze */
	{ 0x456,  0xB3 },	/* cyrillic small letter byelorussian-ukrainian i */
	{ 0x457,  0xBF },	/* cyrillic small letter yi */
	{ 0x458,  0xBC },	/* cyrillic small letter je */
	{ 0x459,  0x9A },	/* cyrillic small letter lje */
	{ 0x45A,  0x9C },	/* cyrillic small letter nje */
	{ 0x45B,  0x9E },	/* cyrillic small letter tshe */
	{ 0x45C,  0x9D },	/* cyrillic small letter kje */
	{ 0x45E,  0xA2 },	/* cyrillic small letter short u */
	{ 0x45F,  0x9F },	/* cyrillic small letter dzhe */
	{ 0x490,  0xA5 },	/* cyrillic capital letter ghe with upturn */
	{ 0x491,  0xB4 },	/* cyrillic small letter ghe with upturn */
	{ 0x2013, 0x96 },	/* en dash */
	{ 0x2014, 0x97 },	/* em dash */
	{ 0x2018, 0x91 },	/* left single quotation mark */
	{ 0x2019, 0x92 },	/* right single quotation mark */
	{ 0x201A, 0x82 },	/* single low-9 quotation mark */
	{ 0x201C, 0x93 },	/* left double quotation mark */
	{ 0x201D, 0x94 },	/* right double quotation mark */
	{ 0x201E, 0x84 },	/* double low-9 quotation mark */
	{ 0x2020, 0x86 },	/* dagger */
	{ 0x2021, 0x87 },	/* double dagger */
	{ 0x2022, 0x95 },	/* bullet */
	{ 0x2026, 0x85 },	/* horizontal ellipsis */
	{ 0x2030, 0x89 },	/* per mille sign */
	{ 0x2039, 0x8B },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0x9B },	/* single right-pointing angle quotation mark */
	{ 0x20AC, 0x88 },	/* euro sign */
	{ 0x2116, 0xB9 },	/* numero sign */
	{ 0x2122, 0x99 },	/* trade mark sign */
};

static const unicode_mapping unimap_koi8r[] = {
	{ 0xA0,   0x9A },	/* no-break space */
	{ 0xA9,   0xBF },	/* copyright sign */
	{ 0xB0,   0x9C },	/* degree sign */
	{ 0xB2,   0x9D },	/* superscript two */
	{ 0xB7,   0x9E },	/* middle dot */
	{ 0xF7,   0x9F },	/* division sign */
	{ 0x401,  0xB3 },	/* cyrillic capital letter io */
	{ 0x410,  0xE1 },	/* cyrillic capital letter a */
	{ 0x411,  0xE2 },	/* cyrillic capital letter be */
	{ 0x412,  0xF7 },	/* cyrillic capital letter ve */
	{ 0x413,  0xE7 },	/* cyrillic capital letter ghe */
	{ 0x414,  0xE4 },	/* cyrillic capital letter de */
	{ 0x415,  0xE5 },	/* cyrillic capital letter ie */
	{ 0x416,  0xF6 },	/* cyrillic capital letter zhe */
	{ 0x417,  0xFA },	/* cyrillic capital letter ze */
	{ 0x418,  0xE9 },	/* cyrillic capital letter i */
	{ 0x419,  0xEA },	/* cyrillic capital letter short i */
	{ 0x41A,  0xEB },	/* cyrillic capital letter ka */
	{ 0x41B,  0xEC },	/* cyrillic capital letter el */
	{ 0x41C,  0xED },	/* cyrillic capital letter em */
	{ 0x41D,  0xEE },	/* cyrillic capital letter en */
	{ 0x41E,  0xEF },	/* cyrillic capital letter o */
	{ 0x41F,  0xF0 },	/* cyrillic capital letter pe */
	{ 0x420,  0xF2 },	/* cyrillic capital letter er */
	{ 0x421,  0xF3 },	/* cyrillic capital letter es */
	{ 0x422,  0xF4 },	/* cyrillic capital letter te */
	{ 0x423,  0xF5 },	/* cyrillic capital letter u */
	{ 0x424,  0xE6 },	/* cyrillic capital letter ef */
	{ 0x425,  0xE8 },	/* cyrillic capital letter ha */
	{ 0x426,  0xE3 },	/* cyrillic capital letter tse */
	{ 0x427,  0xFE },	/* cyrillic capital letter che */
	{ 0x428,  0xFB },	/* cyrillic capital letter sha */
	{ 0x429,  0xFD },	/* cyrillic capital letter shcha */
	{ 0x42A,  0xFF },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0xF9 },	/* cyrillic capital letter yeru */
	{ 0x42C,  0xF8 },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0xFC },	/* cyrillic capital letter e */
	{ 0x42E,  0xE0 },	/* cyrillic capital letter yu */
	{ 0x42F,  0xF1 },	/* cyrillic capital letter ya */
	{ 0x430,  0xC1 },	/* cyrillic small letter a */
	{ 0x431,  0xC2 },	/* cyrillic small letter be */
	{ 0x432,  0xD7 },	/* cyrillic small letter ve */
	{ 0x433,  0xC7 },	/* cyrillic small letter ghe */
	{ 0x434,  0xC4 },	/* cyrillic small letter de */
	{ 0x435,  0xC5 },	/* cyrillic small letter ie */
	{ 0x436,  0xD6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xDA },	/* cyrillic small letter ze */
	{ 0x438,  0xC9 },	/* cyrillic small letter i */
	{ 0x439,  0xCA },	/* cyrillic small letter short i */
	{ 0x43A,  0xCB },	/* cyrillic small letter ka */
	{ 0x43B,  0xCC },	/* cyrillic small letter el */
	{ 0x43C,  0xCD },	/* cyrillic small letter em */
	{ 0x43D,  0xCE },	/* cyrillic small letter en */
	{ 0x43E,  0xCF },	/* cyrillic small letter o */
	{ 0x43F,  0xD0 },	/* cyrillic small letter pe */
	{ 0x440,  0xD2 },	/* cyrillic small letter er */
	{ 0x441,  0xD3 },	/* cyrillic small letter es */
	{ 0x442,  0xD4 },	/* cyrillic small letter te */
	{ 0x443,  0xD5 },	/* cyrillic small letter u */
	{ 0x444,  0xC6 },	/* cyrillic small letter ef */
	{ 0x445,  0xC8 },	/* cyrillic small letter ha */
	{ 0x446,  0xC3 },	/* cyrillic small letter tse */
	{ 0x447,  0xDE },	/* cyrillic small letter che */
	{ 0x448,  0xDB },	/* cyrillic small letter sha */
	{ 0x449,  0xDD },	/* cyrillic small letter shcha */
	{ 0x44A,  0xDF },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xD9 },	/* cyrillic small letter yeru */
	{ 0x44C,  0xD8 },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xDC },	/* cyrillic small letter e */
	{ 0x44E,  0xC0 },	/* cyrillic small letter yu */
	{ 0x44F,  0xD1 },	/* cyrillic small letter ya */
	{ 0x451,  0xA3 },	/* cyrillic small letter io */
	{ 0x2219, 0x95 },	/* bullet operator */
	{ 0x221A, 0x96 },	/* square root */
	{ 0x2248, 0x97 },	/* almost equal to */
	{ 0x2264, 0x98 },	/* less-than or equal to */
	{ 0x2265, 0x99 },	/* greater-than or equal to */
	{ 0x2320, 0x93 },	/* top half integral */
	{ 0x2321, 0x9B },	/* bottom half integral */
	{ 0x2500, 0x80 },	/* box drawings light horizontal */
	{ 0x2502, 0x81 },	/* box drawings light vertical */
	{ 0x250C, 0x82 },	/* box drawings light down and right */
	{ 0x2510, 0x83 },	/* box drawings light down and left */
	{ 0x2514, 0x84 },	/* box drawings light up and right */
	{ 0x2518, 0x85 },	/* box drawings light up and left */
	{ 0x251C, 0x86 },	/* box drawings light vertical and right */
	{ 0x2524, 0x87 },	/* box drawings light vertical and left */
	{ 0x252C, 0x88 },	/* box drawings light down and horizontal */
	{ 0x2534, 0x89 },	/* box drawings light up and horizontal */
	{ 0x253C, 0x8A },	/* box drawings light vertical and horizontal */
	{ 0x2550, 0xA0 },	/* box drawings double horizontal */
	{ 0x2551, 0xA1 },	/* box drawings double vertical */
	{ 0x2552, 0xA2 },	/* box drawings down single and right double */
	{ 0x2553, 0xA4 },	/* box drawings down double and right single */
	{ 0x2554, 0xA5 },	/* box drawings double down and right */
	{ 0x2555, 0xA6 },	/* box drawings down single and left double */
	{ 0x2556, 0xA7 },	/* box drawings down double and left single */
	{ 0x2557, 0xA8 },	/* box drawings double down and left */
	{ 0x2558, 0xA9 },	/* box drawings up single and right double */
	{ 0x2559, 0xAA },	/* box drawings up double and right single */
	{ 0x255A, 0xAB },	/* box drawings double up and right */
	{ 0x255B, 0xAC },	/* box drawings up single and left double */
	{ 0x255C, 0xAD },	/* box drawings up double and left single */
	{ 0x255D, 0xAE },	/* box drawings double up and left */
	{ 0x255E, 0xAF },	/* box drawings vertical single and right double */
	{ 0x255F, 0xB0 },	/* box drawings vertical double and right single */
	{ 0x2560, 0xB1 },	/* box drawings double vertical and right */
	{ 0x2561, 0xB2 },	/* box drawings vertical single and left double */
	{ 0x2562, 0xB4 },	/* box drawings vertical double and left single */
	{ 0x2563, 0xB5 },	/* box drawings double vertical and left */
	{ 0x2564, 0xB6 },	/* box drawings down single and horizontal double */
	{ 0x2565, 0xB7 },	/* box drawings down double and horizontal single */
	{ 0x2566, 0xB8 },	/* box drawings double down and horizontal */
	{ 0x2567, 0xB9 },	/* box drawings up single and horizontal double */
	{ 0x2568, 0xBA },	/* box drawings up double and horizontal single */
	{ 0x2569, 0xBB },	/* box drawings double up and horizontal */
	{ 0x256A, 0xBC },	/* box drawings vertical single and horizontal double */
	{ 0x256B, 0xBD },	/* box drawings vertical double and horizontal single */
	{ 0x256C, 0xBE },	/* box drawings double vertical and horizontal */
	{ 0x2580, 0x8B },	/* upper half block */
	{ 0x2584, 0x8C },	/* lower half block */
	{ 0x2588, 0x8D },	/* full block */
	{ 0x258C, 0x8E },	/* left half block */
	{ 0x2590, 0x8F },	/* right half block */
	{ 0x2591, 0x90 },	/* light shade */
	{ 0x2592, 0x91 },	/* medium shade */
	{ 0x2593, 0x92 },	/* dark shade */
	{ 0x25A0, 0x94 },	/* black square */
};

static const unicode_mapping unimap_cp866[] = {
	{ 0xA0,   0xFF },	/* no-break space */
	{ 0xA4,   0xFD },	/* currency sign */
	{ 0xB0,   0xF8 },	/* degree sign */
	{ 0xB7,   0xFA },	/* middle dot */
	{ 0x401,  0xF0 },	/* cyrillic capital letter io */
	{ 0x404,  0xF2 },	/* cyrillic capital letter ukrainian ie */
	{ 0x407,  0xF4 },	/* cyrillic capital letter yi */
	{ 0x40E,  0xF6 },	/* cyrillic capital letter short u */
	{ 0x410,  0x80 },	/* cyrillic capital letter a */
	{ 0x411,  0x81 },	/* cyrillic capital letter be */
	{ 0x412,  0x82 },	/* cyrillic capital letter ve */
	{ 0x413,  0x83 },	/* cyrillic capital letter ghe */
	{ 0x414,  0x84 },	/* cyrillic capital letter de */
	{ 0x415,  0x85 },	/* cyrillic capital letter ie */
	{ 0x416,  0x86 },	/* cyrillic capital letter zhe */
	{ 0x417,  0x87 },	/* cyrillic capital letter ze */
	{ 0x418,  0x88 },	/* cyrillic capital letter i */
	{ 0x419,  0x89 },	/* cyrillic capital letter short i */
	{ 0x41A,  0x8A },	/* cyrillic capital letter ka */
	{ 0x41B,  0x8B },	/* cyrillic capital letter el */
	{ 0x41C,  0x8C },	/* cyrillic capital letter em */
	{ 0x41D,  0x8D },	/* cyrillic capital letter en */
	{ 0x41E,  0x8E },	/* cyrillic capital letter o */
	{ 0x41F,  0x8F },	/* cyrillic capital letter pe */
	{ 0x420,  0x90 },	/* cyrillic capital letter er */
	{ 0x421,  0x91 },	/* cyrillic capital letter es */
	{ 0x422,  0x92 },	/* cyrillic capital letter te */
	{ 0x423,  0x93 },	/* cyrillic capital letter u */
	{ 0x424,  0x94 },	/* cyrillic capital letter ef */
	{ 0x425,  0x95 },	/* cyrillic capital letter ha */
	{ 0x426,  0x96 },	/* cyrillic capital letter tse */
	{ 0x427,  0x97 },	/* cyrillic capital letter che */
	{ 0x428,  0x98 },	/* cyrillic capital letter sha */
	{ 0x429,  0x99 },	/* cyrillic capital letter shcha */
	{ 0x42A,  0x9A },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0x9B },	/* cyrillic capital letter yeru */
	{ 0x42C,  0x9C },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0x9D },	/* cyrillic capital letter e */
	{ 0x42E,  0x9F },	/* cyrillic capital letter ya */
	{ 0x430,  0xA0 },	/* cyrillic small letter a */
	{ 0x431,  0xA1 },	/* cyrillic small letter be */
	{ 0x432,  0xA2 },	/* cyrillic small letter ve */
	{ 0x433,  0xA3 },	/* cyrillic small letter ghe */
	{ 0x434,  0xA4 },	/* cyrillic small letter de */
	{ 0x435,  0xA5 },	/* cyrillic small letter ie */
	{ 0x436,  0xA6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xA7 },	/* cyrillic small letter ze */
	{ 0x438,  0xA8 },	/* cyrillic small letter i */
	{ 0x439,  0xA9 },	/* cyrillic small letter short i */
	{ 0x43A,  0xAA },	/* cyrillic small letter ka */
	{ 0x43B,  0xAB },	/* cyrillic small letter el */
	{ 0x43C,  0xAC },	/* cyrillic small letter em */
	{ 0x43D,  0xAD },	/* cyrillic small letter en */
	{ 0x43E,  0xAE },	/* cyrillic small letter o */
	{ 0x43F,  0xAF },	/* cyrillic small letter pe */
	{ 0x440,  0xE0 },	/* cyrillic small letter er */
	{ 0x441,  0xE1 },	/* cyrillic small letter es */
	{ 0x442,  0xE2 },	/* cyrillic small letter te */
	{ 0x443,  0xE3 },	/* cyrillic small letter u */
	{ 0x444,  0xE4 },	/* cyrillic small letter ef */
	{ 0x445,  0xE5 },	/* cyrillic small letter ha */
	{ 0x446,  0xE6 },	/* cyrillic small letter tse */
	{ 0x447,  0xE7 },	/* cyrillic small letter che */
	{ 0x448,  0xE8 },	/* cyrillic small letter sha */
	{ 0x449,  0xE9 },	/* cyrillic small letter shcha */
	{ 0x44A,  0xEA },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xEB },	/* cyrillic small letter yeru */
	{ 0x44C,  0xEC },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xED },	/* cyrillic small letter e */
	{ 0x44E,  0xEE },	/* cyrillic small letter yu */
	{ 0x44F,  0xEF },	/* cyrillic small letter ya */
	{ 0x451,  0xF1 },	/* cyrillic small letter io */
	{ 0x454,  0xF3 },	/* cyrillic small letter ukrainian ie */
	{ 0x457,  0xF5 },	/* cyrillic small letter yi */
	{ 0x45E,  0xF7 },	/* cyrillic small letter short u */
	{ 0x2116, 0xFC },	/* numero sign */
	{ 0x2219, 0xF9 },	/* bullet operator */
	{ 0x221A, 0xFB },	/* square root */
	{ 0x2500, 0xC4 },	/* box drawings light horizontal */
	{ 0x2502, 0xB3 },	/* box drawings light vertical */
	{ 0x250C, 0xDA },	/* box drawings light down and right */
	{ 0x2510, 0xBF },	/* box drawings light down and left */
	{ 0x2514, 0xC0 },	/* box drawings light up and right */
	{ 0x2518, 0xD9 },	/* box drawings light up and left */
	{ 0x251C, 0xC3 },	/* box drawings light vertical and right */
	{ 0x2524, 0xB4 },	/* box drawings light vertical and left */
	{ 0x252C, 0xC2 },	/* box drawings light down and horizontal */
	{ 0x2534, 0xC1 },	/* box drawings light up and horizontal */
	{ 0x253C, 0xC5 },	/* box drawings light vertical and horizontal */
	{ 0x2550, 0xCD },	/* box drawings double horizontal */
	{ 0x2551, 0xBA },	/* box drawings double vertical */
	{ 0x2552, 0xD5 },	/* box drawings down single and right double */
	{ 0x2553, 0xD6 },	/* box drawings down double and right single */
	{ 0x2554, 0xC9 },	/* box drawings double down and right */
	{ 0x2555, 0xB8 },	/* box drawings down single and left double */
	{ 0x2556, 0xB7 },	/* box drawings down double and left single */
	{ 0x2557, 0xBB },	/* box drawings double down and left */
	{ 0x2558, 0xD4 },	/* box drawings up single and right double */
	{ 0x2559, 0xD3 },	/* box drawings up double and right single */
	{ 0x255A, 0xC8 },	/* box drawings double up and right */
	{ 0x255B, 0xBE },	/* box drawings up single and left double */
	{ 0x255C, 0xBD },	/* box drawings up double and left single */
	{ 0x255D, 0xBC },	/* box drawings double up and left */
	{ 0x255E, 0xC6 },	/* box drawings vertical single and right double */
	{ 0x255F, 0xC7 },	/* box drawings vertical double and right single */
	{ 0x2560, 0xCC },	/* box drawings double vertical and right */
	{ 0x2561, 0xB5 },	/* box drawings vertical single and left double */
	{ 0x2562, 0xB6 },	/* box drawings vertical double and left single */
	{ 0x2563, 0xB9 },	/* box drawings double vertical and left */
	{ 0x2564, 0xD1 },	/* box drawings down single and horizontal double */
	{ 0x2565, 0xD2 },	/* box drawings down double and horizontal single */
	{ 0x2566, 0xCB },	/* box drawings double down and horizontal */
	{ 0x2567, 0xCF },	/* box drawings up single and horizontal double */
	{ 0x2568, 0xD0 },	/* box drawings up double and horizontal single */
	{ 0x2569, 0xCA },	/* box drawings double up and horizontal */
	{ 0x256A, 0xD8 },	/* box drawings vertical single and horizontal double */
	{ 0x256B, 0xD7 },	/* box drawings vertical double and horizontal single */
	{ 0x256C, 0xCE },	/* box drawings double vertical and horizontal */
	{ 0x2580, 0xDF },	/* upper half block */
	{ 0x2584, 0xDC },	/* lower half block */
	{ 0x2588, 0xDB },	/* full block */
	{ 0x258C, 0xDD },	/* left half block */
	{ 0x2590, 0xDE },	/* right half block */
	{ 0x2591, 0xB0 },	/* light shade */
	{ 0x2592, 0xB1 },	/* medium shade */
	{ 0x2593, 0xB2 },	/* dark shade */
	{ 0x25A0, 0xFE },	/* black square */
};

static const unicode_mapping unimap_macroman[] = {
	{ 0xA0,   0xCA },	/* no-break space */
	{ 0xA1,   0xC1 },	/* inverted exclamation mark */
	{ 0xA2,   0xA2 },	/* cent sign */
	{ 0xA3,   0xA3 },	/* pound sign */
	{ 0xA5,   0xB4 },	/* yen sign */
	{ 0xA7,   0xA4 },	/* section sign */
	{ 0xA8,   0xAC },	/* diaeresis */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAA,   0xBB },	/* feminine ordinal indicator */
	{ 0xAB,   0xC7 },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xC2 },	/* not sign */
	{ 0xAE,   0xA8 },	/* registered sign */
	{ 0xAF,   0xF8 },	/* macron */
	{ 0xB0,   0xA1 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB4,   0xAB },	/* acute accent */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xA6 },	/* pilcrow sign */
	{ 0xB7,   0xE1 },	/* middle dot */
	{ 0xB8,   0xFC },	/* cedilla */
	{ 0xBA,   0xBC },	/* masculine ordinal indicator */
	{ 0xBB,   0xC8 },	/* right-pointing double angle quotation mark */
	{ 0xBF,   0xC0 },	/* inverted question mark */
	{ 0xC0,   0xCB },	/* latin capital letter a with grave */
	{ 0xC1,   0xE7 },	/* latin capital letter a with acute */
	{ 0xC2,   0xE5 },	/* latin capital letter a with circumflex */
	{ 0xC3,   0xCC },	/* latin capital letter a with tilde */
	{ 0xC4,   0x80 },	/* latin capital letter a with diaeresis */
	{ 0xC5,   0x81 },	/* latin capital letter a with ring above */
	{ 0xC6,   0xAE },	/* latin capital letter ae */
	{ 0xC7,   0x82 },	/* latin capital letter c with cedilla */
	{ 0xC8,   0xE9 },	/* latin capital letter e with grave */
	{ 0xC9,   0x83 },	/* latin capital letter e with acute */
	{ 0xCA,   0xE6 },	/* latin capital letter e with circumflex */
	{ 0xCB,   0xE8 },	/* latin capital letter e with diaeresis */
	{ 0xCC,   0xED },	/* latin capital letter i with grave */
	{ 0xCD,   0xEA },	/* latin capital letter i with acute */
	{ 0xCE,   0xEB },	/* latin capital letter i with circumflex */
	{ 0xCF,   0xEC },	/* latin capital letter i with diaeresis */
	{ 0xD1,   0x84 },	/* latin capital letter n with tilde */
	{ 0xD2,   0xF1 },	/* latin capital letter o with grave */
	{ 0xD3,   0xEE },	/* latin capital letter o with acute */
	{ 0xD4,   0xEF },	/* latin capital letter o with circumflex */
	{ 0xD5,   0xCD },	/* latin capital letter o with tilde */
	{ 0xD6,   0x85 },	/* latin capital letter o with diaeresis */
	{ 0xD8,   0xAF },	/* latin capital letter o with stroke */
	{ 0xD9,   0xF4 },	/* latin capital letter u with grave */
	{ 0xDA,   0xF2 },	/* latin capital letter u with acute */
	{ 0xDB,   0xF3 },	/* latin capital letter u with circumflex */
	{ 0xDC,   0x86 },	/* latin capital letter u with diaeresis */
	{ 0xDF,   0xA7 },	/* latin small letter sharp s */
	{ 0xE0,   0x88 },	/* latin small letter a with grave */
	{ 0xE1,   0x87 },	/* latin small letter a with acute */
	{ 0xE2,   0x89 },	/* latin small letter a with circumflex */
	{ 0xE3,   0x8B },	/* latin small letter a with tilde */
	{ 0xE4,   0x8A },	/* latin small letter a with diaeresis */
	{ 0xE5,   0x8C },	/* latin small letter a with ring above */
	{ 0xE6,   0xBE },	/* latin small letter ae */
	{ 0xE7,   0x8D },	/* latin small letter c with cedilla */
	{ 0xE8,   0x8F },	/* latin small letter e with grave */
	{ 0xE9,   0x8E },	/* latin small letter e with acute */
	{ 0xEA,   0x90 },	/* latin small letter e with circumflex */
	{ 0xEB,   0x91 },	/* latin small letter e with diaeresis */
	{ 0xEC,   0x93 },	/* latin small letter i with grave */
	{ 0xED,   0x92 },	/* latin small letter i with acute */
	{ 0xEE,   0x94 },	/* latin small letter i with circumflex */
	{ 0xEF,   0x95 },	/* latin small letter i with diaeresis */
	{ 0xF1,   0x96 },	/* latin small letter n with tilde */
	{ 0xF2,   0x98 },	/* latin small letter o with grave */
	{ 0xF3,   0x97 },	/* latin small letter o with acute */
	{ 0xF4,   0x99 },	/* latin small letter o with circumflex */
	{ 0xF5,   0x9B },	/* latin small letter o with tilde */
	{ 0xF6,   0x9A },	/* latin small letter o with diaeresis */
	{ 0xF7,   0xD6 },	/* division sign */
	{ 0xF8,   0xBF },	/* latin small letter o with stroke */
	{ 0xF9,   0x9D },	/* latin small letter u with grave */
	{ 0xFA,   0x9C },	/* latin small letter u with acute */
	{ 0xFB,   0x9E },	/* latin small letter u with circumflex */
	{ 0xFC,   0x9F },	/* latin small letter u with diaeresis */
	{ 0xFF,   0xD8 },	/* latin small letter y with diaeresis */
	{ 0x131,  0xF5 },	/* latin small letter dotless i */
	{ 0x152,  0xCE },	/* latin capital ligature oe */
	{ 0x153,  0xCF },	/* latin small ligature oe */
	{ 0x178,  0xD9 },	/* latin capital letter y with diaeresis */
	{ 0x192,  0xC4 },	/* latin small letter f with hook */
	{ 0x2C6,  0xF6 },	/* modifier letter circumflex accent */
	{ 0x2C7,  0xFF },	/* caron */
	{ 0x2D8,  0xF9 },	/* breve */
	{ 0x2D9,  0xFA },	/* dot above */
	{ 0x2DA,  0xFB },	/* ring above */
	{ 0x2DB,  0xFE },	/* ogonek */
	{ 0x2DC,  0xF7 },	/* small tilde */
	{ 0x2DD,  0xFD },	/* double acute accent */
	{ 0x3A9,  0xBD },	/* greek capital letter omega */
	{ 0x3C0,  0xB9 },	/* greek small letter pi */
	{ 0x2013, 0xD0 },	/* en dash */
	{ 0x2014, 0xD1 },	/* em dash */
	{ 0x2018, 0xD4 },	/* left single quotation mark */
	{ 0x2019, 0xD5 },	/* right single quotation mark */
	{ 0x201A, 0xE2 },	/* single low-9 quotation mark */
	{ 0x201C, 0xD2 },	/* left double quotation mark */
	{ 0x201D, 0xD3 },	/* right double quotation mark */
	{ 0x201E, 0xE3 },	/* double low-9 quotation mark */
	{ 0x2020, 0xA0 },	/* dagger */
	{ 0x2021, 0xE0 },	/* double dagger */
	{ 0x2022, 0xA5 },	/* bullet */
	{ 0x2026, 0xC9 },	/* horizontal ellipsis */
	{ 0x2030, 0xE4 },	/* per mille sign */
	{ 0x2039, 0xDC },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0xDD },	/* single right-pointing angle quotation mark */
	{ 0x2044, 0xDA },	/* fraction slash */
	{ 0x20AC, 0xDB },	/* euro sign */
	{ 0x2122, 0xAA },	/* trade mark sign */
	{ 0x2202, 0xB6 },	/* partial differential */
	{ 0x2206, 0xC6 },	/* increment */
	{ 0x220F, 0xB8 },	/* n-ary product */
	{ 0x2211, 0xB7 },	/* n-ary summation */
	{ 0x221A, 0xC3 },	/* square root */
	{ 0x221E, 0xB0 },	/* infinity */
	{ 0x222B, 0xBA },	/* integral */
	{ 0x2248, 0xC5 },	/* almost equal to */
	{ 0x2260, 0xAD },	/* not equal to */
	{ 0x2264, 0xB2 },	/* less-than or equal to */
	{ 0x2265, 0xB3 },	/* greater-than or equal to */
	{ 0x25CA, 0xD7 },	/* lozenge */
	{ 0xF8FF, 0xF0 },	/* apple logo */
	{ 0xFB01, 0xDE },	/* latin small ligature fi */
	{ 0xFB02, 0xDF },	/* latin small ligature fl */
};

#endif /* HTML_TABLES_H */
/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id: html.h 293036 2010-01-03 09:23:27Z sebastian $ */

#ifndef HTML_TABLES_H
#define HTML_TABLES_H

/* cs_terminator is overloaded in the following fashion:
 * - It terminates the list entity maps.
 * - In BG(inverse_ent_maps), it's the key of the inverse map that stores
 *   only the basic entities.
 * - When passed to traverse_for_entities (or via php_unescape_entities with !all),
 *   we don't care about the encoding (UTF-8 is chosen, but it should be used
 *   when it doesn't matter).
 */
enum entity_charset { cs_terminator, cs_8859_1, cs_cp1252,
					  cs_8859_15, cs_utf_8, cs_big5, cs_gb2312, 
					  cs_big5hkscs, cs_sjis, cs_eucjp, cs_koi8r,
					  cs_cp1251, cs_8859_5, cs_cp866, cs_macroman,
					  cs_numelems /* used to count the number of charsets */
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
	"dagger", "Dagger", "bull", NULL, NULL, NULL, "hellip",
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
	"lArr", "uArr", "rArr", "dArr", "hArr", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8672 (0x21e0) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8704 (0x2200) */
	"forall", NULL, "part", "exist", NULL, "empty", NULL, "nabla",
	"isin", "notin", NULL, "ni", NULL, NULL, NULL, "prod",
	/* 8720 (0x2210) */
	NULL, "sum", "minus", NULL, NULL, NULL, NULL, "lowast",
	NULL, NULL, "radic", NULL, NULL, "prop", "infin", NULL,
	/* 8736 (0x2220) */
	"ang", NULL, NULL, NULL, NULL, NULL, NULL, "and",
	"or", "cap", "cup", "int", NULL, NULL, NULL, NULL,
	/* 8752 (0x2230) */
	NULL, NULL, NULL, NULL, "there4", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, "sim", NULL, NULL, NULL,
	/* 8768 (0x2240) */
	NULL, NULL, NULL, NULL, NULL, "cong", NULL, NULL,
	"asymp", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8784 (0x2250) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8800 (0x2260) */
	"ne", "equiv", NULL, NULL, "le", "ge", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8816 (0x2270) */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	/* 8832 (0x2280) */
	NULL, NULL, "sub", "sup", "nsub", NULL, "sube", "supe",
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
	unsigned int basechar;			/* char code at start of table */
	unsigned int endchar;			/* last char code in the table */
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

typedef struct {
	unsigned short charcode;
	char *entity;
	int entitylen;
	int flags;
} basic_entity_t;

static const basic_entity_t basic_entities_ex[] = {
	{ '&',  "&amp;",    5,  0 },
	{ '"',	"&quot;",	6,	ENT_HTML_QUOTE_DOUBLE },
	/* PHP traditionally encodes ' as &#039;, not &apos;, so leave this entry here */
	{ '\'',	"&#039;",	6,	ENT_HTML_QUOTE_SINGLE },
	{ '\'',	"&apos;",	6,	ENT_HTML_QUOTE_SINGLE },
	{ '<',	"&lt;",		4,	0 },
	{ '>',	"&gt;",		4,	0 },
	{ 0, NULL, 0, 0 }
};

/* In some cases, we need to give special treatment to &, so we
 * use this instead */
static const basic_entity_t *basic_entities = &basic_entities_ex[1];

typedef struct {
	unsigned short un_code_point; /* we don't need bigger */
	unsigned char cs_code; /* currently, we only have maps to single-byte encodings */
} unicode_mapping;

static const unicode_mapping unimap_iso885915[] = {
	{ 0xA5,   0xA5 },	/* yen sign */
	{ 0xA7,   0xA7 },	/* section sign */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAA,   0xAA },	/* feminine ordinal indicator */
	{ 0xAB,   0xAB },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xAC },	/* not sign */
	{ 0xAD,   0xAD },	/* soft hyphen */
	{ 0xAE,   0xAE },	/* registered sign */
	{ 0xAF,   0xAF },	/* macron */
	{ 0xB0,   0xB0 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB2,   0xB2 },	/* superscript two */
	{ 0xB3,   0xB3 },	/* superscript three */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xB6 },	/* pilcrow sign */
	{ 0xB7,   0xB7 },	/* middle dot */
	{ 0xB9,   0xB9 },	/* superscript one */
	{ 0xBA,   0xBA },	/* masculine ordinal indicator */
	{ 0xBB,   0xBB },	/* right-pointing double angle quotation mark */
	{ 0x152,  0xBC },	/* latin capital ligature oe */
	{ 0x153,  0xBD },	/* latin small ligature oe */
	{ 0x160,  0xA6 },	/* latin capital letter s with caron */
	{ 0x161,  0xA8 },	/* latin small letter s with caron */
	{ 0x178,  0xBE },	/* latin capital letter y with diaeresis */
	{ 0x17D,  0xB4 },	/* latin capital letter z with caron */
	{ 0x17E,  0xB8 },	/* latin small letter z with caron */
	{ 0x20AC, 0xA4 },	/* euro sign */
};

static const unicode_mapping unimap_win1252[] = {
	{ 0x152,  0x8C },	/* latin capital ligature oe */
	{ 0x153,  0x9C },	/* latin small ligature oe */
	{ 0x160,  0x8A },	/* latin capital letter s with caron */
	{ 0x161,  0x9A },	/* latin small letter s with caron */
	{ 0x178,  0x9F },	/* latin capital letter y with diaeresis */
	{ 0x17D,  0x8E },	/* latin capital letter z with caron */
	{ 0x17E,  0x9E },	/* latin small letter z with caron */
	{ 0x192,  0x83 },	/* latin small letter f with hook */
	{ 0x2C6,  0x88 },	/* modifier letter circumflex accent */
	{ 0x2DC,  0x98 },	/* small tilde */
	{ 0x2013, 0x96 },	/* en dash */
	{ 0x2014, 0x97 },	/* em dash */
	{ 0x2018, 0x91 },	/* left single quotation mark */
	{ 0x2019, 0x92 },	/* right single quotation mark */
	{ 0x201A, 0x82 },	/* single low-9 quotation mark */
	{ 0x201C, 0x93 },	/* left double quotation mark */
	{ 0x201D, 0x94 },	/* right double quotation mark */
	{ 0x201E, 0x84 },	/* double low-9 quotation mark */
	{ 0x2020, 0x86 },	/* dagger */
	{ 0x2021, 0x87 },	/* double dagger */
	{ 0x2022, 0x95 },	/* bullet */
	{ 0x2026, 0x85 },	/* horizontal ellipsis */
	{ 0x2030, 0x89 },	/* per mille sign */
	{ 0x2039, 0x8B },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0x9B },	/* single right-pointing angle quotation mark */
	{ 0x20AC, 0x80 },	/* euro sign */
	{ 0x2122, 0x99 },	/* trade mark sign */
};

static const unicode_mapping unimap_win1251[] = {
	{ 0xA0,   0xA0 },	/* no-break space */
	{ 0xA4,   0xA4 },	/* currency sign */
	{ 0xA6,   0xA6 },	/* broken bar */
	{ 0xA7,   0xA7 },	/* section sign */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAB,   0xAB },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xAC },	/* not sign */
	{ 0xAD,   0xAD },	/* soft hyphen */
	{ 0xAE,   0xAE },	/* registered sign */
	{ 0xB0,   0xB0 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xB6 },	/* pilcrow sign */
	{ 0xB7,   0xB7 },	/* middle dot */
	{ 0xBB,   0xBB },	/* right-pointing double angle quotation mark */
	{ 0x401,  0xA8 },	/* cyrillic capital letter io */
	{ 0x402,  0x80 },	/* cyrillic capital letter dje */
	{ 0x403,  0x81 },	/* cyrillic capital letter gje */
	{ 0x404,  0xAA },	/* cyrillic capital letter ukrainian ie */
	{ 0x405,  0xBD },	/* cyrillic capital letter dze */
	{ 0x406,  0xB2 },	/* cyrillic capital letter byelorussian-ukrainian i */
	{ 0x407,  0xAF },	/* cyrillic capital letter yi */
	{ 0x408,  0xA3 },	/* cyrillic capital letter je */
	{ 0x409,  0x8A },	/* cyrillic capital letter lje */
	{ 0x40A,  0x8C },	/* cyrillic capital letter nje */
	{ 0x40B,  0x8E },	/* cyrillic capital letter tshe */
	{ 0x40C,  0x8D },	/* cyrillic capital letter kje */
	{ 0x40E,  0xA1 },	/* cyrillic capital letter short u */
	{ 0x40F,  0x8F },	/* cyrillic capital letter dzhe */
	{ 0x410,  0xC0 },	/* cyrillic capital letter a */
	{ 0x411,  0xC1 },	/* cyrillic capital letter be */
	{ 0x412,  0xC2 },	/* cyrillic capital letter ve */
	{ 0x413,  0xC3 },	/* cyrillic capital letter ghe */
	{ 0x414,  0xC4 },	/* cyrillic capital letter de */
	{ 0x415,  0xC5 },	/* cyrillic capital letter ie */
	{ 0x416,  0xC6 },	/* cyrillic capital letter zhe */
	{ 0x417,  0xC7 },	/* cyrillic capital letter ze */
	{ 0x418,  0xC8 },	/* cyrillic capital letter i */
	{ 0x419,  0xC9 },	/* cyrillic capital letter short i */
	{ 0x41A,  0xCA },	/* cyrillic capital letter ka */
	{ 0x41B,  0xCB },	/* cyrillic capital letter el */
	{ 0x41C,  0xCC },	/* cyrillic capital letter em */
	{ 0x41D,  0xCD },	/* cyrillic capital letter en */
	{ 0x41E,  0xCE },	/* cyrillic capital letter o */
	{ 0x41F,  0xCF },	/* cyrillic capital letter pe */
	{ 0x420,  0xD0 },	/* cyrillic capital letter er */
	{ 0x421,  0xD1 },	/* cyrillic capital letter es */
	{ 0x422,  0xD2 },	/* cyrillic capital letter te */
	{ 0x423,  0xD3 },	/* cyrillic capital letter u */
	{ 0x424,  0xD4 },	/* cyrillic capital letter ef */
	{ 0x425,  0xD5 },	/* cyrillic capital letter ha */
	{ 0x426,  0xD6 },	/* cyrillic capital letter tse */
	{ 0x427,  0xD7 },	/* cyrillic capital letter che */
	{ 0x428,  0xD8 },	/* cyrillic capital letter sha */
	{ 0x429,  0xD9 },	/* cyrillic capital letter shcha */
	{ 0x42A,  0xDA },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0xDB },	/* cyrillic capital letter yeru */
	{ 0x42C,  0xDC },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0xDD },	/* cyrillic capital letter e */
	{ 0x42E,  0xDE },	/* cyrillic capital letter yu */
	{ 0x42F,  0xDF },	/* cyrillic capital letter ya */
	{ 0x430,  0xE0 },	/* cyrillic small letter a */
	{ 0x431,  0xE1 },	/* cyrillic small letter be */
	{ 0x432,  0xE2 },	/* cyrillic small letter ve */
	{ 0x433,  0xE3 },	/* cyrillic small letter ghe */
	{ 0x434,  0xE4 },	/* cyrillic small letter de */
	{ 0x435,  0xE5 },	/* cyrillic small letter ie */
	{ 0x436,  0xE6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xE7 },	/* cyrillic small letter ze */
	{ 0x438,  0xE8 },	/* cyrillic small letter i */
	{ 0x439,  0xE9 },	/* cyrillic small letter short i */
	{ 0x43A,  0xEA },	/* cyrillic small letter ka */
	{ 0x43B,  0xEB },	/* cyrillic small letter el */
	{ 0x43C,  0xEC },	/* cyrillic small letter em */
	{ 0x43D,  0xED },	/* cyrillic small letter en */
	{ 0x43E,  0xEE },	/* cyrillic small letter o */
	{ 0x43F,  0xEF },	/* cyrillic small letter pe */
	{ 0x440,  0xF0 },	/* cyrillic small letter er */
	{ 0x441,  0xF1 },	/* cyrillic small letter es */
	{ 0x442,  0xF2 },	/* cyrillic small letter te */
	{ 0x443,  0xF3 },	/* cyrillic small letter u */
	{ 0x444,  0xF4 },	/* cyrillic small letter ef */
	{ 0x445,  0xF5 },	/* cyrillic small letter ha */
	{ 0x446,  0xF6 },	/* cyrillic small letter tse */
	{ 0x447,  0xF7 },	/* cyrillic small letter che */
	{ 0x448,  0xF8 },	/* cyrillic small letter sha */
	{ 0x449,  0xF9 },	/* cyrillic small letter shcha */
	{ 0x44A,  0xFA },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xFB },	/* cyrillic small letter yeru */
	{ 0x44C,  0xFC },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xFD },	/* cyrillic small letter e */
	{ 0x44E,  0xFE },	/* cyrillic small letter yu */
	{ 0x44F,  0xFF },	/* cyrillic small letter ya */
	{ 0x451,  0xB8 },	/* cyrillic small letter io */
	{ 0x452,  0x90 },	/* cyrillic small letter dje */
	{ 0x453,  0x83 },	/* cyrillic small letter gje */
	{ 0x454,  0xBA },	/* cyrillic small letter ukrainian ie */
	{ 0x455,  0xBE },	/* cyrillic small letter dze */
	{ 0x456,  0xB3 },	/* cyrillic small letter byelorussian-ukrainian i */
	{ 0x457,  0xBF },	/* cyrillic small letter yi */
	{ 0x458,  0xBC },	/* cyrillic small letter je */
	{ 0x459,  0x9A },	/* cyrillic small letter lje */
	{ 0x45A,  0x9C },	/* cyrillic small letter nje */
	{ 0x45B,  0x9E },	/* cyrillic small letter tshe */
	{ 0x45C,  0x9D },	/* cyrillic small letter kje */
	{ 0x45E,  0xA2 },	/* cyrillic small letter short u */
	{ 0x45F,  0x9F },	/* cyrillic small letter dzhe */
	{ 0x490,  0xA5 },	/* cyrillic capital letter ghe with upturn */
	{ 0x491,  0xB4 },	/* cyrillic small letter ghe with upturn */
	{ 0x2013, 0x96 },	/* en dash */
	{ 0x2014, 0x97 },	/* em dash */
	{ 0x2018, 0x91 },	/* left single quotation mark */
	{ 0x2019, 0x92 },	/* right single quotation mark */
	{ 0x201A, 0x82 },	/* single low-9 quotation mark */
	{ 0x201C, 0x93 },	/* left double quotation mark */
	{ 0x201D, 0x94 },	/* right double quotation mark */
	{ 0x201E, 0x84 },	/* double low-9 quotation mark */
	{ 0x2020, 0x86 },	/* dagger */
	{ 0x2021, 0x87 },	/* double dagger */
	{ 0x2022, 0x95 },	/* bullet */
	{ 0x2026, 0x85 },	/* horizontal ellipsis */
	{ 0x2030, 0x89 },	/* per mille sign */
	{ 0x2039, 0x8B },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0x9B },	/* single right-pointing angle quotation mark */
	{ 0x20AC, 0x88 },	/* euro sign */
	{ 0x2116, 0xB9 },	/* numero sign */
	{ 0x2122, 0x99 },	/* trade mark sign */
};

static const unicode_mapping unimap_koi8r[] = {
	{ 0xA0,   0x9A },	/* no-break space */
	{ 0xA9,   0xBF },	/* copyright sign */
	{ 0xB0,   0x9C },	/* degree sign */
	{ 0xB2,   0x9D },	/* superscript two */
	{ 0xB7,   0x9E },	/* middle dot */
	{ 0xF7,   0x9F },	/* division sign */
	{ 0x401,  0xB3 },	/* cyrillic capital letter io */
	{ 0x410,  0xE1 },	/* cyrillic capital letter a */
	{ 0x411,  0xE2 },	/* cyrillic capital letter be */
	{ 0x412,  0xF7 },	/* cyrillic capital letter ve */
	{ 0x413,  0xE7 },	/* cyrillic capital letter ghe */
	{ 0x414,  0xE4 },	/* cyrillic capital letter de */
	{ 0x415,  0xE5 },	/* cyrillic capital letter ie */
	{ 0x416,  0xF6 },	/* cyrillic capital letter zhe */
	{ 0x417,  0xFA },	/* cyrillic capital letter ze */
	{ 0x418,  0xE9 },	/* cyrillic capital letter i */
	{ 0x419,  0xEA },	/* cyrillic capital letter short i */
	{ 0x41A,  0xEB },	/* cyrillic capital letter ka */
	{ 0x41B,  0xEC },	/* cyrillic capital letter el */
	{ 0x41C,  0xED },	/* cyrillic capital letter em */
	{ 0x41D,  0xEE },	/* cyrillic capital letter en */
	{ 0x41E,  0xEF },	/* cyrillic capital letter o */
	{ 0x41F,  0xF0 },	/* cyrillic capital letter pe */
	{ 0x420,  0xF2 },	/* cyrillic capital letter er */
	{ 0x421,  0xF3 },	/* cyrillic capital letter es */
	{ 0x422,  0xF4 },	/* cyrillic capital letter te */
	{ 0x423,  0xF5 },	/* cyrillic capital letter u */
	{ 0x424,  0xE6 },	/* cyrillic capital letter ef */
	{ 0x425,  0xE8 },	/* cyrillic capital letter ha */
	{ 0x426,  0xE3 },	/* cyrillic capital letter tse */
	{ 0x427,  0xFE },	/* cyrillic capital letter che */
	{ 0x428,  0xFB },	/* cyrillic capital letter sha */
	{ 0x429,  0xFD },	/* cyrillic capital letter shcha */
	{ 0x42A,  0xFF },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0xF9 },	/* cyrillic capital letter yeru */
	{ 0x42C,  0xF8 },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0xFC },	/* cyrillic capital letter e */
	{ 0x42E,  0xE0 },	/* cyrillic capital letter yu */
	{ 0x42F,  0xF1 },	/* cyrillic capital letter ya */
	{ 0x430,  0xC1 },	/* cyrillic small letter a */
	{ 0x431,  0xC2 },	/* cyrillic small letter be */
	{ 0x432,  0xD7 },	/* cyrillic small letter ve */
	{ 0x433,  0xC7 },	/* cyrillic small letter ghe */
	{ 0x434,  0xC4 },	/* cyrillic small letter de */
	{ 0x435,  0xC5 },	/* cyrillic small letter ie */
	{ 0x436,  0xD6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xDA },	/* cyrillic small letter ze */
	{ 0x438,  0xC9 },	/* cyrillic small letter i */
	{ 0x439,  0xCA },	/* cyrillic small letter short i */
	{ 0x43A,  0xCB },	/* cyrillic small letter ka */
	{ 0x43B,  0xCC },	/* cyrillic small letter el */
	{ 0x43C,  0xCD },	/* cyrillic small letter em */
	{ 0x43D,  0xCE },	/* cyrillic small letter en */
	{ 0x43E,  0xCF },	/* cyrillic small letter o */
	{ 0x43F,  0xD0 },	/* cyrillic small letter pe */
	{ 0x440,  0xD2 },	/* cyrillic small letter er */
	{ 0x441,  0xD3 },	/* cyrillic small letter es */
	{ 0x442,  0xD4 },	/* cyrillic small letter te */
	{ 0x443,  0xD5 },	/* cyrillic small letter u */
	{ 0x444,  0xC6 },	/* cyrillic small letter ef */
	{ 0x445,  0xC8 },	/* cyrillic small letter ha */
	{ 0x446,  0xC3 },	/* cyrillic small letter tse */
	{ 0x447,  0xDE },	/* cyrillic small letter che */
	{ 0x448,  0xDB },	/* cyrillic small letter sha */
	{ 0x449,  0xDD },	/* cyrillic small letter shcha */
	{ 0x44A,  0xDF },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xD9 },	/* cyrillic small letter yeru */
	{ 0x44C,  0xD8 },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xDC },	/* cyrillic small letter e */
	{ 0x44E,  0xC0 },	/* cyrillic small letter yu */
	{ 0x44F,  0xD1 },	/* cyrillic small letter ya */
	{ 0x451,  0xA3 },	/* cyrillic small letter io */
	{ 0x2219, 0x95 },	/* bullet operator */
	{ 0x221A, 0x96 },	/* square root */
	{ 0x2248, 0x97 },	/* almost equal to */
	{ 0x2264, 0x98 },	/* less-than or equal to */
	{ 0x2265, 0x99 },	/* greater-than or equal to */
	{ 0x2320, 0x93 },	/* top half integral */
	{ 0x2321, 0x9B },	/* bottom half integral */
	{ 0x2500, 0x80 },	/* box drawings light horizontal */
	{ 0x2502, 0x81 },	/* box drawings light vertical */
	{ 0x250C, 0x82 },	/* box drawings light down and right */
	{ 0x2510, 0x83 },	/* box drawings light down and left */
	{ 0x2514, 0x84 },	/* box drawings light up and right */
	{ 0x2518, 0x85 },	/* box drawings light up and left */
	{ 0x251C, 0x86 },	/* box drawings light vertical and right */
	{ 0x2524, 0x87 },	/* box drawings light vertical and left */
	{ 0x252C, 0x88 },	/* box drawings light down and horizontal */
	{ 0x2534, 0x89 },	/* box drawings light up and horizontal */
	{ 0x253C, 0x8A },	/* box drawings light vertical and horizontal */
	{ 0x2550, 0xA0 },	/* box drawings double horizontal */
	{ 0x2551, 0xA1 },	/* box drawings double vertical */
	{ 0x2552, 0xA2 },	/* box drawings down single and right double */
	{ 0x2553, 0xA4 },	/* box drawings down double and right single */
	{ 0x2554, 0xA5 },	/* box drawings double down and right */
	{ 0x2555, 0xA6 },	/* box drawings down single and left double */
	{ 0x2556, 0xA7 },	/* box drawings down double and left single */
	{ 0x2557, 0xA8 },	/* box drawings double down and left */
	{ 0x2558, 0xA9 },	/* box drawings up single and right double */
	{ 0x2559, 0xAA },	/* box drawings up double and right single */
	{ 0x255A, 0xAB },	/* box drawings double up and right */
	{ 0x255B, 0xAC },	/* box drawings up single and left double */
	{ 0x255C, 0xAD },	/* box drawings up double and left single */
	{ 0x255D, 0xAE },	/* box drawings double up and left */
	{ 0x255E, 0xAF },	/* box drawings vertical single and right double */
	{ 0x255F, 0xB0 },	/* box drawings vertical double and right single */
	{ 0x2560, 0xB1 },	/* box drawings double vertical and right */
	{ 0x2561, 0xB2 },	/* box drawings vertical single and left double */
	{ 0x2562, 0xB4 },	/* box drawings vertical double and left single */
	{ 0x2563, 0xB5 },	/* box drawings double vertical and left */
	{ 0x2564, 0xB6 },	/* box drawings down single and horizontal double */
	{ 0x2565, 0xB7 },	/* box drawings down double and horizontal single */
	{ 0x2566, 0xB8 },	/* box drawings double down and horizontal */
	{ 0x2567, 0xB9 },	/* box drawings up single and horizontal double */
	{ 0x2568, 0xBA },	/* box drawings up double and horizontal single */
	{ 0x2569, 0xBB },	/* box drawings double up and horizontal */
	{ 0x256A, 0xBC },	/* box drawings vertical single and horizontal double */
	{ 0x256B, 0xBD },	/* box drawings vertical double and horizontal single */
	{ 0x256C, 0xBE },	/* box drawings double vertical and horizontal */
	{ 0x2580, 0x8B },	/* upper half block */
	{ 0x2584, 0x8C },	/* lower half block */
	{ 0x2588, 0x8D },	/* full block */
	{ 0x258C, 0x8E },	/* left half block */
	{ 0x2590, 0x8F },	/* right half block */
	{ 0x2591, 0x90 },	/* light shade */
	{ 0x2592, 0x91 },	/* medium shade */
	{ 0x2593, 0x92 },	/* dark shade */
	{ 0x25A0, 0x94 },	/* black square */
};

static const unicode_mapping unimap_cp866[] = {
	{ 0xA0,   0xFF },	/* no-break space */
	{ 0xA4,   0xFD },	/* currency sign */
	{ 0xB0,   0xF8 },	/* degree sign */
	{ 0xB7,   0xFA },	/* middle dot */
	{ 0x401,  0xF0 },	/* cyrillic capital letter io */
	{ 0x404,  0xF2 },	/* cyrillic capital letter ukrainian ie */
	{ 0x407,  0xF4 },	/* cyrillic capital letter yi */
	{ 0x40E,  0xF6 },	/* cyrillic capital letter short u */
	{ 0x410,  0x80 },	/* cyrillic capital letter a */
	{ 0x411,  0x81 },	/* cyrillic capital letter be */
	{ 0x412,  0x82 },	/* cyrillic capital letter ve */
	{ 0x413,  0x83 },	/* cyrillic capital letter ghe */
	{ 0x414,  0x84 },	/* cyrillic capital letter de */
	{ 0x415,  0x85 },	/* cyrillic capital letter ie */
	{ 0x416,  0x86 },	/* cyrillic capital letter zhe */
	{ 0x417,  0x87 },	/* cyrillic capital letter ze */
	{ 0x418,  0x88 },	/* cyrillic capital letter i */
	{ 0x419,  0x89 },	/* cyrillic capital letter short i */
	{ 0x41A,  0x8A },	/* cyrillic capital letter ka */
	{ 0x41B,  0x8B },	/* cyrillic capital letter el */
	{ 0x41C,  0x8C },	/* cyrillic capital letter em */
	{ 0x41D,  0x8D },	/* cyrillic capital letter en */
	{ 0x41E,  0x8E },	/* cyrillic capital letter o */
	{ 0x41F,  0x8F },	/* cyrillic capital letter pe */
	{ 0x420,  0x90 },	/* cyrillic capital letter er */
	{ 0x421,  0x91 },	/* cyrillic capital letter es */
	{ 0x422,  0x92 },	/* cyrillic capital letter te */
	{ 0x423,  0x93 },	/* cyrillic capital letter u */
	{ 0x424,  0x94 },	/* cyrillic capital letter ef */
	{ 0x425,  0x95 },	/* cyrillic capital letter ha */
	{ 0x426,  0x96 },	/* cyrillic capital letter tse */
	{ 0x427,  0x97 },	/* cyrillic capital letter che */
	{ 0x428,  0x98 },	/* cyrillic capital letter sha */
	{ 0x429,  0x99 },	/* cyrillic capital letter shcha */
	{ 0x42A,  0x9A },	/* cyrillic capital letter hard sign */
	{ 0x42B,  0x9B },	/* cyrillic capital letter yeru */
	{ 0x42C,  0x9C },	/* cyrillic capital letter soft sign */
	{ 0x42D,  0x9D },	/* cyrillic capital letter e */
	{ 0x42E,  0x9F },	/* cyrillic capital letter ya */
	{ 0x430,  0xA0 },	/* cyrillic small letter a */
	{ 0x431,  0xA1 },	/* cyrillic small letter be */
	{ 0x432,  0xA2 },	/* cyrillic small letter ve */
	{ 0x433,  0xA3 },	/* cyrillic small letter ghe */
	{ 0x434,  0xA4 },	/* cyrillic small letter de */
	{ 0x435,  0xA5 },	/* cyrillic small letter ie */
	{ 0x436,  0xA6 },	/* cyrillic small letter zhe */
	{ 0x437,  0xA7 },	/* cyrillic small letter ze */
	{ 0x438,  0xA8 },	/* cyrillic small letter i */
	{ 0x439,  0xA9 },	/* cyrillic small letter short i */
	{ 0x43A,  0xAA },	/* cyrillic small letter ka */
	{ 0x43B,  0xAB },	/* cyrillic small letter el */
	{ 0x43C,  0xAC },	/* cyrillic small letter em */
	{ 0x43D,  0xAD },	/* cyrillic small letter en */
	{ 0x43E,  0xAE },	/* cyrillic small letter o */
	{ 0x43F,  0xAF },	/* cyrillic small letter pe */
	{ 0x440,  0xE0 },	/* cyrillic small letter er */
	{ 0x441,  0xE1 },	/* cyrillic small letter es */
	{ 0x442,  0xE2 },	/* cyrillic small letter te */
	{ 0x443,  0xE3 },	/* cyrillic small letter u */
	{ 0x444,  0xE4 },	/* cyrillic small letter ef */
	{ 0x445,  0xE5 },	/* cyrillic small letter ha */
	{ 0x446,  0xE6 },	/* cyrillic small letter tse */
	{ 0x447,  0xE7 },	/* cyrillic small letter che */
	{ 0x448,  0xE8 },	/* cyrillic small letter sha */
	{ 0x449,  0xE9 },	/* cyrillic small letter shcha */
	{ 0x44A,  0xEA },	/* cyrillic small letter hard sign */
	{ 0x44B,  0xEB },	/* cyrillic small letter yeru */
	{ 0x44C,  0xEC },	/* cyrillic small letter soft sign */
	{ 0x44D,  0xED },	/* cyrillic small letter e */
	{ 0x44E,  0xEE },	/* cyrillic small letter yu */
	{ 0x44F,  0xEF },	/* cyrillic small letter ya */
	{ 0x451,  0xF1 },	/* cyrillic small letter io */
	{ 0x454,  0xF3 },	/* cyrillic small letter ukrainian ie */
	{ 0x457,  0xF5 },	/* cyrillic small letter yi */
	{ 0x45E,  0xF7 },	/* cyrillic small letter short u */
	{ 0x2116, 0xFC },	/* numero sign */
	{ 0x2219, 0xF9 },	/* bullet operator */
	{ 0x221A, 0xFB },	/* square root */
	{ 0x2500, 0xC4 },	/* box drawings light horizontal */
	{ 0x2502, 0xB3 },	/* box drawings light vertical */
	{ 0x250C, 0xDA },	/* box drawings light down and right */
	{ 0x2510, 0xBF },	/* box drawings light down and left */
	{ 0x2514, 0xC0 },	/* box drawings light up and right */
	{ 0x2518, 0xD9 },	/* box drawings light up and left */
	{ 0x251C, 0xC3 },	/* box drawings light vertical and right */
	{ 0x2524, 0xB4 },	/* box drawings light vertical and left */
	{ 0x252C, 0xC2 },	/* box drawings light down and horizontal */
	{ 0x2534, 0xC1 },	/* box drawings light up and horizontal */
	{ 0x253C, 0xC5 },	/* box drawings light vertical and horizontal */
	{ 0x2550, 0xCD },	/* box drawings double horizontal */
	{ 0x2551, 0xBA },	/* box drawings double vertical */
	{ 0x2552, 0xD5 },	/* box drawings down single and right double */
	{ 0x2553, 0xD6 },	/* box drawings down double and right single */
	{ 0x2554, 0xC9 },	/* box drawings double down and right */
	{ 0x2555, 0xB8 },	/* box drawings down single and left double */
	{ 0x2556, 0xB7 },	/* box drawings down double and left single */
	{ 0x2557, 0xBB },	/* box drawings double down and left */
	{ 0x2558, 0xD4 },	/* box drawings up single and right double */
	{ 0x2559, 0xD3 },	/* box drawings up double and right single */
	{ 0x255A, 0xC8 },	/* box drawings double up and right */
	{ 0x255B, 0xBE },	/* box drawings up single and left double */
	{ 0x255C, 0xBD },	/* box drawings up double and left single */
	{ 0x255D, 0xBC },	/* box drawings double up and left */
	{ 0x255E, 0xC6 },	/* box drawings vertical single and right double */
	{ 0x255F, 0xC7 },	/* box drawings vertical double and right single */
	{ 0x2560, 0xCC },	/* box drawings double vertical and right */
	{ 0x2561, 0xB5 },	/* box drawings vertical single and left double */
	{ 0x2562, 0xB6 },	/* box drawings vertical double and left single */
	{ 0x2563, 0xB9 },	/* box drawings double vertical and left */
	{ 0x2564, 0xD1 },	/* box drawings down single and horizontal double */
	{ 0x2565, 0xD2 },	/* box drawings down double and horizontal single */
	{ 0x2566, 0xCB },	/* box drawings double down and horizontal */
	{ 0x2567, 0xCF },	/* box drawings up single and horizontal double */
	{ 0x2568, 0xD0 },	/* box drawings up double and horizontal single */
	{ 0x2569, 0xCA },	/* box drawings double up and horizontal */
	{ 0x256A, 0xD8 },	/* box drawings vertical single and horizontal double */
	{ 0x256B, 0xD7 },	/* box drawings vertical double and horizontal single */
	{ 0x256C, 0xCE },	/* box drawings double vertical and horizontal */
	{ 0x2580, 0xDF },	/* upper half block */
	{ 0x2584, 0xDC },	/* lower half block */
	{ 0x2588, 0xDB },	/* full block */
	{ 0x258C, 0xDD },	/* left half block */
	{ 0x2590, 0xDE },	/* right half block */
	{ 0x2591, 0xB0 },	/* light shade */
	{ 0x2592, 0xB1 },	/* medium shade */
	{ 0x2593, 0xB2 },	/* dark shade */
	{ 0x25A0, 0xFE },	/* black square */
};

static const unicode_mapping unimap_macroman[] = {
	{ 0xA0,   0xCA },	/* no-break space */
	{ 0xA1,   0xC1 },	/* inverted exclamation mark */
	{ 0xA2,   0xA2 },	/* cent sign */
	{ 0xA3,   0xA3 },	/* pound sign */
	{ 0xA5,   0xB4 },	/* yen sign */
	{ 0xA7,   0xA4 },	/* section sign */
	{ 0xA8,   0xAC },	/* diaeresis */
	{ 0xA9,   0xA9 },	/* copyright sign */
	{ 0xAA,   0xBB },	/* feminine ordinal indicator */
	{ 0xAB,   0xC7 },	/* left-pointing double angle quotation mark */
	{ 0xAC,   0xC2 },	/* not sign */
	{ 0xAE,   0xA8 },	/* registered sign */
	{ 0xAF,   0xF8 },	/* macron */
	{ 0xB0,   0xA1 },	/* degree sign */
	{ 0xB1,   0xB1 },	/* plus-minus sign */
	{ 0xB4,   0xAB },	/* acute accent */
	{ 0xB5,   0xB5 },	/* micro sign */
	{ 0xB6,   0xA6 },	/* pilcrow sign */
	{ 0xB7,   0xE1 },	/* middle dot */
	{ 0xB8,   0xFC },	/* cedilla */
	{ 0xBA,   0xBC },	/* masculine ordinal indicator */
	{ 0xBB,   0xC8 },	/* right-pointing double angle quotation mark */
	{ 0xBF,   0xC0 },	/* inverted question mark */
	{ 0xC0,   0xCB },	/* latin capital letter a with grave */
	{ 0xC1,   0xE7 },	/* latin capital letter a with acute */
	{ 0xC2,   0xE5 },	/* latin capital letter a with circumflex */
	{ 0xC3,   0xCC },	/* latin capital letter a with tilde */
	{ 0xC4,   0x80 },	/* latin capital letter a with diaeresis */
	{ 0xC5,   0x81 },	/* latin capital letter a with ring above */
	{ 0xC6,   0xAE },	/* latin capital letter ae */
	{ 0xC7,   0x82 },	/* latin capital letter c with cedilla */
	{ 0xC8,   0xE9 },	/* latin capital letter e with grave */
	{ 0xC9,   0x83 },	/* latin capital letter e with acute */
	{ 0xCA,   0xE6 },	/* latin capital letter e with circumflex */
	{ 0xCB,   0xE8 },	/* latin capital letter e with diaeresis */
	{ 0xCC,   0xED },	/* latin capital letter i with grave */
	{ 0xCD,   0xEA },	/* latin capital letter i with acute */
	{ 0xCE,   0xEB },	/* latin capital letter i with circumflex */
	{ 0xCF,   0xEC },	/* latin capital letter i with diaeresis */
	{ 0xD1,   0x84 },	/* latin capital letter n with tilde */
	{ 0xD2,   0xF1 },	/* latin capital letter o with grave */
	{ 0xD3,   0xEE },	/* latin capital letter o with acute */
	{ 0xD4,   0xEF },	/* latin capital letter o with circumflex */
	{ 0xD5,   0xCD },	/* latin capital letter o with tilde */
	{ 0xD6,   0x85 },	/* latin capital letter o with diaeresis */
	{ 0xD8,   0xAF },	/* latin capital letter o with stroke */
	{ 0xD9,   0xF4 },	/* latin capital letter u with grave */
	{ 0xDA,   0xF2 },	/* latin capital letter u with acute */
	{ 0xDB,   0xF3 },	/* latin capital letter u with circumflex */
	{ 0xDC,   0x86 },	/* latin capital letter u with diaeresis */
	{ 0xDF,   0xA7 },	/* latin small letter sharp s */
	{ 0xE0,   0x88 },	/* latin small letter a with grave */
	{ 0xE1,   0x87 },	/* latin small letter a with acute */
	{ 0xE2,   0x89 },	/* latin small letter a with circumflex */
	{ 0xE3,   0x8B },	/* latin small letter a with tilde */
	{ 0xE4,   0x8A },	/* latin small letter a with diaeresis */
	{ 0xE5,   0x8C },	/* latin small letter a with ring above */
	{ 0xE6,   0xBE },	/* latin small letter ae */
	{ 0xE7,   0x8D },	/* latin small letter c with cedilla */
	{ 0xE8,   0x8F },	/* latin small letter e with grave */
	{ 0xE9,   0x8E },	/* latin small letter e with acute */
	{ 0xEA,   0x90 },	/* latin small letter e with circumflex */
	{ 0xEB,   0x91 },	/* latin small letter e with diaeresis */
	{ 0xEC,   0x93 },	/* latin small letter i with grave */
	{ 0xED,   0x92 },	/* latin small letter i with acute */
	{ 0xEE,   0x94 },	/* latin small letter i with circumflex */
	{ 0xEF,   0x95 },	/* latin small letter i with diaeresis */
	{ 0xF1,   0x96 },	/* latin small letter n with tilde */
	{ 0xF2,   0x98 },	/* latin small letter o with grave */
	{ 0xF3,   0x97 },	/* latin small letter o with acute */
	{ 0xF4,   0x99 },	/* latin small letter o with circumflex */
	{ 0xF5,   0x9B },	/* latin small letter o with tilde */
	{ 0xF6,   0x9A },	/* latin small letter o with diaeresis */
	{ 0xF7,   0xD6 },	/* division sign */
	{ 0xF8,   0xBF },	/* latin small letter o with stroke */
	{ 0xF9,   0x9D },	/* latin small letter u with grave */
	{ 0xFA,   0x9C },	/* latin small letter u with acute */
	{ 0xFB,   0x9E },	/* latin small letter u with circumflex */
	{ 0xFC,   0x9F },	/* latin small letter u with diaeresis */
	{ 0xFF,   0xD8 },	/* latin small letter y with diaeresis */
	{ 0x131,  0xF5 },	/* latin small letter dotless i */
	{ 0x152,  0xCE },	/* latin capital ligature oe */
	{ 0x153,  0xCF },	/* latin small ligature oe */
	{ 0x178,  0xD9 },	/* latin capital letter y with diaeresis */
	{ 0x192,  0xC4 },	/* latin small letter f with hook */
	{ 0x2C6,  0xF6 },	/* modifier letter circumflex accent */
	{ 0x2C7,  0xFF },	/* caron */
	{ 0x2D8,  0xF9 },	/* breve */
	{ 0x2D9,  0xFA },	/* dot above */
	{ 0x2DA,  0xFB },	/* ring above */
	{ 0x2DB,  0xFE },	/* ogonek */
	{ 0x2DC,  0xF7 },	/* small tilde */
	{ 0x2DD,  0xFD },	/* double acute accent */
	{ 0x3A9,  0xBD },	/* greek capital letter omega */
	{ 0x3C0,  0xB9 },	/* greek small letter pi */
	{ 0x2013, 0xD0 },	/* en dash */
	{ 0x2014, 0xD1 },	/* em dash */
	{ 0x2018, 0xD4 },	/* left single quotation mark */
	{ 0x2019, 0xD5 },	/* right single quotation mark */
	{ 0x201A, 0xE2 },	/* single low-9 quotation mark */
	{ 0x201C, 0xD2 },	/* left double quotation mark */
	{ 0x201D, 0xD3 },	/* right double quotation mark */
	{ 0x201E, 0xE3 },	/* double low-9 quotation mark */
	{ 0x2020, 0xA0 },	/* dagger */
	{ 0x2021, 0xE0 },	/* double dagger */
	{ 0x2022, 0xA5 },	/* bullet */
	{ 0x2026, 0xC9 },	/* horizontal ellipsis */
	{ 0x2030, 0xE4 },	/* per mille sign */
	{ 0x2039, 0xDC },	/* single left-pointing angle quotation mark */
	{ 0x203A, 0xDD },	/* single right-pointing angle quotation mark */
	{ 0x2044, 0xDA },	/* fraction slash */
	{ 0x20AC, 0xDB },	/* euro sign */
	{ 0x2122, 0xAA },	/* trade mark sign */
	{ 0x2202, 0xB6 },	/* partial differential */
	{ 0x2206, 0xC6 },	/* increment */
	{ 0x220F, 0xB8 },	/* n-ary product */
	{ 0x2211, 0xB7 },	/* n-ary summation */
	{ 0x221A, 0xC3 },	/* square root */
	{ 0x221E, 0xB0 },	/* infinity */
	{ 0x222B, 0xBA },	/* integral */
	{ 0x2248, 0xC5 },	/* almost equal to */
	{ 0x2260, 0xAD },	/* not equal to */
	{ 0x2264, 0xB2 },	/* less-than or equal to */
	{ 0x2265, 0xB3 },	/* greater-than or equal to */
	{ 0x25CA, 0xD7 },	/* lozenge */
	{ 0xF8FF, 0xF0 },	/* apple logo */
	{ 0xFB01, 0xDE },	/* latin small ligature fi */
	{ 0xFB02, 0xDF },	/* latin small ligature fl */
};

#endif /* HTML_TABLES_H */
