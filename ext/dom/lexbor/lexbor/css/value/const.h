/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

/*
 * Caution!
 * This file generated by the script "utils/lexbor/css/names.py"!
 * Do not change this file!
 */


#ifndef LXB_CSS_VALUE_CONST_H
#define LXB_CSS_VALUE_CONST_H


enum {
    LXB_CSS_VALUE__UNDEF               = 0x0000,
    LXB_CSS_VALUE_INITIAL              = 0x0001,
    LXB_CSS_VALUE_INHERIT              = 0x0002,
    LXB_CSS_VALUE_UNSET                = 0x0003,
    LXB_CSS_VALUE_REVERT               = 0x0004,
    LXB_CSS_VALUE_FLEX_START           = 0x0005,
    LXB_CSS_VALUE_FLEX_END             = 0x0006,
    LXB_CSS_VALUE_CENTER               = 0x0007,
    LXB_CSS_VALUE_SPACE_BETWEEN        = 0x0008,
    LXB_CSS_VALUE_SPACE_AROUND         = 0x0009,
    LXB_CSS_VALUE_STRETCH              = 0x000a,
    LXB_CSS_VALUE_BASELINE             = 0x000b,
    LXB_CSS_VALUE_AUTO                 = 0x000c,
    LXB_CSS_VALUE_TEXT_BOTTOM          = 0x000d,
    LXB_CSS_VALUE_ALPHABETIC           = 0x000e,
    LXB_CSS_VALUE_IDEOGRAPHIC          = 0x000f,
    LXB_CSS_VALUE_MIDDLE               = 0x0010,
    LXB_CSS_VALUE_CENTRAL              = 0x0011,
    LXB_CSS_VALUE_MATHEMATICAL         = 0x0012,
    LXB_CSS_VALUE_TEXT_TOP             = 0x0013,
    LXB_CSS_VALUE__LENGTH              = 0x0014,
    LXB_CSS_VALUE__PERCENTAGE          = 0x0015,
    LXB_CSS_VALUE_SUB                  = 0x0016,
    LXB_CSS_VALUE_SUPER                = 0x0017,
    LXB_CSS_VALUE_TOP                  = 0x0018,
    LXB_CSS_VALUE_BOTTOM               = 0x0019,
    LXB_CSS_VALUE_FIRST                = 0x001a,
    LXB_CSS_VALUE_LAST                 = 0x001b,
    LXB_CSS_VALUE_THIN                 = 0x001c,
    LXB_CSS_VALUE_MEDIUM               = 0x001d,
    LXB_CSS_VALUE_THICK                = 0x001e,
    LXB_CSS_VALUE_NONE                 = 0x001f,
    LXB_CSS_VALUE_HIDDEN               = 0x0020,
    LXB_CSS_VALUE_DOTTED               = 0x0021,
    LXB_CSS_VALUE_DASHED               = 0x0022,
    LXB_CSS_VALUE_SOLID                = 0x0023,
    LXB_CSS_VALUE_DOUBLE               = 0x0024,
    LXB_CSS_VALUE_GROOVE               = 0x0025,
    LXB_CSS_VALUE_RIDGE                = 0x0026,
    LXB_CSS_VALUE_INSET                = 0x0027,
    LXB_CSS_VALUE_OUTSET               = 0x0028,
    LXB_CSS_VALUE_CONTENT_BOX          = 0x0029,
    LXB_CSS_VALUE_BORDER_BOX           = 0x002a,
    LXB_CSS_VALUE_INLINE_START         = 0x002b,
    LXB_CSS_VALUE_INLINE_END           = 0x002c,
    LXB_CSS_VALUE_BLOCK_START          = 0x002d,
    LXB_CSS_VALUE_BLOCK_END            = 0x002e,
    LXB_CSS_VALUE_LEFT                 = 0x002f,
    LXB_CSS_VALUE_RIGHT                = 0x0030,
    LXB_CSS_VALUE_CURRENTCOLOR         = 0x0031,
    LXB_CSS_VALUE_TRANSPARENT          = 0x0032,
    LXB_CSS_VALUE_HEX                  = 0x0033,
    LXB_CSS_VALUE_ALICEBLUE            = 0x0034,
    LXB_CSS_VALUE_ANTIQUEWHITE         = 0x0035,
    LXB_CSS_VALUE_AQUA                 = 0x0036,
    LXB_CSS_VALUE_AQUAMARINE           = 0x0037,
    LXB_CSS_VALUE_AZURE                = 0x0038,
    LXB_CSS_VALUE_BEIGE                = 0x0039,
    LXB_CSS_VALUE_BISQUE               = 0x003a,
    LXB_CSS_VALUE_BLACK                = 0x003b,
    LXB_CSS_VALUE_BLANCHEDALMOND       = 0x003c,
    LXB_CSS_VALUE_BLUE                 = 0x003d,
    LXB_CSS_VALUE_BLUEVIOLET           = 0x003e,
    LXB_CSS_VALUE_BROWN                = 0x003f,
    LXB_CSS_VALUE_BURLYWOOD            = 0x0040,
    LXB_CSS_VALUE_CADETBLUE            = 0x0041,
    LXB_CSS_VALUE_CHARTREUSE           = 0x0042,
    LXB_CSS_VALUE_CHOCOLATE            = 0x0043,
    LXB_CSS_VALUE_CORAL                = 0x0044,
    LXB_CSS_VALUE_CORNFLOWERBLUE       = 0x0045,
    LXB_CSS_VALUE_CORNSILK             = 0x0046,
    LXB_CSS_VALUE_CRIMSON              = 0x0047,
    LXB_CSS_VALUE_CYAN                 = 0x0048,
    LXB_CSS_VALUE_DARKBLUE             = 0x0049,
    LXB_CSS_VALUE_DARKCYAN             = 0x004a,
    LXB_CSS_VALUE_DARKGOLDENROD        = 0x004b,
    LXB_CSS_VALUE_DARKGRAY             = 0x004c,
    LXB_CSS_VALUE_DARKGREEN            = 0x004d,
    LXB_CSS_VALUE_DARKGREY             = 0x004e,
    LXB_CSS_VALUE_DARKKHAKI            = 0x004f,
    LXB_CSS_VALUE_DARKMAGENTA          = 0x0050,
    LXB_CSS_VALUE_DARKOLIVEGREEN       = 0x0051,
    LXB_CSS_VALUE_DARKORANGE           = 0x0052,
    LXB_CSS_VALUE_DARKORCHID           = 0x0053,
    LXB_CSS_VALUE_DARKRED              = 0x0054,
    LXB_CSS_VALUE_DARKSALMON           = 0x0055,
    LXB_CSS_VALUE_DARKSEAGREEN         = 0x0056,
    LXB_CSS_VALUE_DARKSLATEBLUE        = 0x0057,
    LXB_CSS_VALUE_DARKSLATEGRAY        = 0x0058,
    LXB_CSS_VALUE_DARKSLATEGREY        = 0x0059,
    LXB_CSS_VALUE_DARKTURQUOISE        = 0x005a,
    LXB_CSS_VALUE_DARKVIOLET           = 0x005b,
    LXB_CSS_VALUE_DEEPPINK             = 0x005c,
    LXB_CSS_VALUE_DEEPSKYBLUE          = 0x005d,
    LXB_CSS_VALUE_DIMGRAY              = 0x005e,
    LXB_CSS_VALUE_DIMGREY              = 0x005f,
    LXB_CSS_VALUE_DODGERBLUE           = 0x0060,
    LXB_CSS_VALUE_FIREBRICK            = 0x0061,
    LXB_CSS_VALUE_FLORALWHITE          = 0x0062,
    LXB_CSS_VALUE_FORESTGREEN          = 0x0063,
    LXB_CSS_VALUE_FUCHSIA              = 0x0064,
    LXB_CSS_VALUE_GAINSBORO            = 0x0065,
    LXB_CSS_VALUE_GHOSTWHITE           = 0x0066,
    LXB_CSS_VALUE_GOLD                 = 0x0067,
    LXB_CSS_VALUE_GOLDENROD            = 0x0068,
    LXB_CSS_VALUE_GRAY                 = 0x0069,
    LXB_CSS_VALUE_GREEN                = 0x006a,
    LXB_CSS_VALUE_GREENYELLOW          = 0x006b,
    LXB_CSS_VALUE_GREY                 = 0x006c,
    LXB_CSS_VALUE_HONEYDEW             = 0x006d,
    LXB_CSS_VALUE_HOTPINK              = 0x006e,
    LXB_CSS_VALUE_INDIANRED            = 0x006f,
    LXB_CSS_VALUE_INDIGO               = 0x0070,
    LXB_CSS_VALUE_IVORY                = 0x0071,
    LXB_CSS_VALUE_KHAKI                = 0x0072,
    LXB_CSS_VALUE_LAVENDER             = 0x0073,
    LXB_CSS_VALUE_LAVENDERBLUSH        = 0x0074,
    LXB_CSS_VALUE_LAWNGREEN            = 0x0075,
    LXB_CSS_VALUE_LEMONCHIFFON         = 0x0076,
    LXB_CSS_VALUE_LIGHTBLUE            = 0x0077,
    LXB_CSS_VALUE_LIGHTCORAL           = 0x0078,
    LXB_CSS_VALUE_LIGHTCYAN            = 0x0079,
    LXB_CSS_VALUE_LIGHTGOLDENRODYELLOW = 0x007a,
    LXB_CSS_VALUE_LIGHTGRAY            = 0x007b,
    LXB_CSS_VALUE_LIGHTGREEN           = 0x007c,
    LXB_CSS_VALUE_LIGHTGREY            = 0x007d,
    LXB_CSS_VALUE_LIGHTPINK            = 0x007e,
    LXB_CSS_VALUE_LIGHTSALMON          = 0x007f,
    LXB_CSS_VALUE_LIGHTSEAGREEN        = 0x0080,
    LXB_CSS_VALUE_LIGHTSKYBLUE         = 0x0081,
    LXB_CSS_VALUE_LIGHTSLATEGRAY       = 0x0082,
    LXB_CSS_VALUE_LIGHTSLATEGREY       = 0x0083,
    LXB_CSS_VALUE_LIGHTSTEELBLUE       = 0x0084,
    LXB_CSS_VALUE_LIGHTYELLOW          = 0x0085,
    LXB_CSS_VALUE_LIME                 = 0x0086,
    LXB_CSS_VALUE_LIMEGREEN            = 0x0087,
    LXB_CSS_VALUE_LINEN                = 0x0088,
    LXB_CSS_VALUE_MAGENTA              = 0x0089,
    LXB_CSS_VALUE_MAROON               = 0x008a,
    LXB_CSS_VALUE_MEDIUMAQUAMARINE     = 0x008b,
    LXB_CSS_VALUE_MEDIUMBLUE           = 0x008c,
    LXB_CSS_VALUE_MEDIUMORCHID         = 0x008d,
    LXB_CSS_VALUE_MEDIUMPURPLE         = 0x008e,
    LXB_CSS_VALUE_MEDIUMSEAGREEN       = 0x008f,
    LXB_CSS_VALUE_MEDIUMSLATEBLUE      = 0x0090,
    LXB_CSS_VALUE_MEDIUMSPRINGGREEN    = 0x0091,
    LXB_CSS_VALUE_MEDIUMTURQUOISE      = 0x0092,
    LXB_CSS_VALUE_MEDIUMVIOLETRED      = 0x0093,
    LXB_CSS_VALUE_MIDNIGHTBLUE         = 0x0094,
    LXB_CSS_VALUE_MINTCREAM            = 0x0095,
    LXB_CSS_VALUE_MISTYROSE            = 0x0096,
    LXB_CSS_VALUE_MOCCASIN             = 0x0097,
    LXB_CSS_VALUE_NAVAJOWHITE          = 0x0098,
    LXB_CSS_VALUE_NAVY                 = 0x0099,
    LXB_CSS_VALUE_OLDLACE              = 0x009a,
    LXB_CSS_VALUE_OLIVE                = 0x009b,
    LXB_CSS_VALUE_OLIVEDRAB            = 0x009c,
    LXB_CSS_VALUE_ORANGE               = 0x009d,
    LXB_CSS_VALUE_ORANGERED            = 0x009e,
    LXB_CSS_VALUE_ORCHID               = 0x009f,
    LXB_CSS_VALUE_PALEGOLDENROD        = 0x00a0,
    LXB_CSS_VALUE_PALEGREEN            = 0x00a1,
    LXB_CSS_VALUE_PALETURQUOISE        = 0x00a2,
    LXB_CSS_VALUE_PALEVIOLETRED        = 0x00a3,
    LXB_CSS_VALUE_PAPAYAWHIP           = 0x00a4,
    LXB_CSS_VALUE_PEACHPUFF            = 0x00a5,
    LXB_CSS_VALUE_PERU                 = 0x00a6,
    LXB_CSS_VALUE_PINK                 = 0x00a7,
    LXB_CSS_VALUE_PLUM                 = 0x00a8,
    LXB_CSS_VALUE_POWDERBLUE           = 0x00a9,
    LXB_CSS_VALUE_PURPLE               = 0x00aa,
    LXB_CSS_VALUE_REBECCAPURPLE        = 0x00ab,
    LXB_CSS_VALUE_RED                  = 0x00ac,
    LXB_CSS_VALUE_ROSYBROWN            = 0x00ad,
    LXB_CSS_VALUE_ROYALBLUE            = 0x00ae,
    LXB_CSS_VALUE_SADDLEBROWN          = 0x00af,
    LXB_CSS_VALUE_SALMON               = 0x00b0,
    LXB_CSS_VALUE_SANDYBROWN           = 0x00b1,
    LXB_CSS_VALUE_SEAGREEN             = 0x00b2,
    LXB_CSS_VALUE_SEASHELL             = 0x00b3,
    LXB_CSS_VALUE_SIENNA               = 0x00b4,
    LXB_CSS_VALUE_SILVER               = 0x00b5,
    LXB_CSS_VALUE_SKYBLUE              = 0x00b6,
    LXB_CSS_VALUE_SLATEBLUE            = 0x00b7,
    LXB_CSS_VALUE_SLATEGRAY            = 0x00b8,
    LXB_CSS_VALUE_SLATEGREY            = 0x00b9,
    LXB_CSS_VALUE_SNOW                 = 0x00ba,
    LXB_CSS_VALUE_SPRINGGREEN          = 0x00bb,
    LXB_CSS_VALUE_STEELBLUE            = 0x00bc,
    LXB_CSS_VALUE_TAN                  = 0x00bd,
    LXB_CSS_VALUE_TEAL                 = 0x00be,
    LXB_CSS_VALUE_THISTLE              = 0x00bf,
    LXB_CSS_VALUE_TOMATO               = 0x00c0,
    LXB_CSS_VALUE_TURQUOISE            = 0x00c1,
    LXB_CSS_VALUE_VIOLET               = 0x00c2,
    LXB_CSS_VALUE_WHEAT                = 0x00c3,
    LXB_CSS_VALUE_WHITE                = 0x00c4,
    LXB_CSS_VALUE_WHITESMOKE           = 0x00c5,
    LXB_CSS_VALUE_YELLOW               = 0x00c6,
    LXB_CSS_VALUE_YELLOWGREEN          = 0x00c7,
    LXB_CSS_VALUE_CANVAS               = 0x00c8,
    LXB_CSS_VALUE_CANVASTEXT           = 0x00c9,
    LXB_CSS_VALUE_LINKTEXT             = 0x00ca,
    LXB_CSS_VALUE_VISITEDTEXT          = 0x00cb,
    LXB_CSS_VALUE_ACTIVETEXT           = 0x00cc,
    LXB_CSS_VALUE_BUTTONFACE           = 0x00cd,
    LXB_CSS_VALUE_BUTTONTEXT           = 0x00ce,
    LXB_CSS_VALUE_BUTTONBORDER         = 0x00cf,
    LXB_CSS_VALUE_FIELD                = 0x00d0,
    LXB_CSS_VALUE_FIELDTEXT            = 0x00d1,
    LXB_CSS_VALUE_HIGHLIGHT            = 0x00d2,
    LXB_CSS_VALUE_HIGHLIGHTTEXT        = 0x00d3,
    LXB_CSS_VALUE_SELECTEDITEM         = 0x00d4,
    LXB_CSS_VALUE_SELECTEDITEMTEXT     = 0x00d5,
    LXB_CSS_VALUE_MARK                 = 0x00d6,
    LXB_CSS_VALUE_MARKTEXT             = 0x00d7,
    LXB_CSS_VALUE_GRAYTEXT             = 0x00d8,
    LXB_CSS_VALUE_ACCENTCOLOR          = 0x00d9,
    LXB_CSS_VALUE_ACCENTCOLORTEXT      = 0x00da,
    LXB_CSS_VALUE_RGB                  = 0x00db,
    LXB_CSS_VALUE_RGBA                 = 0x00dc,
    LXB_CSS_VALUE_HSL                  = 0x00dd,
    LXB_CSS_VALUE_HSLA                 = 0x00de,
    LXB_CSS_VALUE_HWB                  = 0x00df,
    LXB_CSS_VALUE_LAB                  = 0x00e0,
    LXB_CSS_VALUE_LCH                  = 0x00e1,
    LXB_CSS_VALUE_OKLAB                = 0x00e2,
    LXB_CSS_VALUE_OKLCH                = 0x00e3,
    LXB_CSS_VALUE_COLOR                = 0x00e4,
    LXB_CSS_VALUE_LTR                  = 0x00e5,
    LXB_CSS_VALUE_RTL                  = 0x00e6,
    LXB_CSS_VALUE_BLOCK                = 0x00e7,
    LXB_CSS_VALUE_INLINE               = 0x00e8,
    LXB_CSS_VALUE_RUN_IN               = 0x00e9,
    LXB_CSS_VALUE_FLOW                 = 0x00ea,
    LXB_CSS_VALUE_FLOW_ROOT            = 0x00eb,
    LXB_CSS_VALUE_TABLE                = 0x00ec,
    LXB_CSS_VALUE_FLEX                 = 0x00ed,
    LXB_CSS_VALUE_GRID                 = 0x00ee,
    LXB_CSS_VALUE_RUBY                 = 0x00ef,
    LXB_CSS_VALUE_LIST_ITEM            = 0x00f0,
    LXB_CSS_VALUE_TABLE_ROW_GROUP      = 0x00f1,
    LXB_CSS_VALUE_TABLE_HEADER_GROUP   = 0x00f2,
    LXB_CSS_VALUE_TABLE_FOOTER_GROUP   = 0x00f3,
    LXB_CSS_VALUE_TABLE_ROW            = 0x00f4,
    LXB_CSS_VALUE_TABLE_CELL           = 0x00f5,
    LXB_CSS_VALUE_TABLE_COLUMN_GROUP   = 0x00f6,
    LXB_CSS_VALUE_TABLE_COLUMN         = 0x00f7,
    LXB_CSS_VALUE_TABLE_CAPTION        = 0x00f8,
    LXB_CSS_VALUE_RUBY_BASE            = 0x00f9,
    LXB_CSS_VALUE_RUBY_TEXT            = 0x00fa,
    LXB_CSS_VALUE_RUBY_BASE_CONTAINER  = 0x00fb,
    LXB_CSS_VALUE_RUBY_TEXT_CONTAINER  = 0x00fc,
    LXB_CSS_VALUE_CONTENTS             = 0x00fd,
    LXB_CSS_VALUE_INLINE_BLOCK         = 0x00fe,
    LXB_CSS_VALUE_INLINE_TABLE         = 0x00ff,
    LXB_CSS_VALUE_INLINE_FLEX          = 0x0100,
    LXB_CSS_VALUE_INLINE_GRID          = 0x0101,
    LXB_CSS_VALUE_HANGING              = 0x0102,
    LXB_CSS_VALUE_CONTENT              = 0x0103,
    LXB_CSS_VALUE_ROW                  = 0x0104,
    LXB_CSS_VALUE_ROW_REVERSE          = 0x0105,
    LXB_CSS_VALUE_COLUMN               = 0x0106,
    LXB_CSS_VALUE_COLUMN_REVERSE       = 0x0107,
    LXB_CSS_VALUE__NUMBER              = 0x0108,
    LXB_CSS_VALUE_NOWRAP               = 0x0109,
    LXB_CSS_VALUE_WRAP                 = 0x010a,
    LXB_CSS_VALUE_WRAP_REVERSE         = 0x010b,
    LXB_CSS_VALUE_SNAP_BLOCK           = 0x010c,
    LXB_CSS_VALUE_START                = 0x010d,
    LXB_CSS_VALUE_END                  = 0x010e,
    LXB_CSS_VALUE_NEAR                 = 0x010f,
    LXB_CSS_VALUE_SNAP_INLINE          = 0x0110,
    LXB_CSS_VALUE__INTEGER             = 0x0111,
    LXB_CSS_VALUE_REGION               = 0x0112,
    LXB_CSS_VALUE_PAGE                 = 0x0113,
    LXB_CSS_VALUE_SERIF                = 0x0114,
    LXB_CSS_VALUE_SANS_SERIF           = 0x0115,
    LXB_CSS_VALUE_CURSIVE              = 0x0116,
    LXB_CSS_VALUE_FANTASY              = 0x0117,
    LXB_CSS_VALUE_MONOSPACE            = 0x0118,
    LXB_CSS_VALUE_SYSTEM_UI            = 0x0119,
    LXB_CSS_VALUE_EMOJI                = 0x011a,
    LXB_CSS_VALUE_MATH                 = 0x011b,
    LXB_CSS_VALUE_FANGSONG             = 0x011c,
    LXB_CSS_VALUE_UI_SERIF             = 0x011d,
    LXB_CSS_VALUE_UI_SANS_SERIF        = 0x011e,
    LXB_CSS_VALUE_UI_MONOSPACE         = 0x011f,
    LXB_CSS_VALUE_UI_ROUNDED           = 0x0120,
    LXB_CSS_VALUE_XX_SMALL             = 0x0121,
    LXB_CSS_VALUE_X_SMALL              = 0x0122,
    LXB_CSS_VALUE_SMALL                = 0x0123,
    LXB_CSS_VALUE_LARGE                = 0x0124,
    LXB_CSS_VALUE_X_LARGE              = 0x0125,
    LXB_CSS_VALUE_XX_LARGE             = 0x0126,
    LXB_CSS_VALUE_XXX_LARGE            = 0x0127,
    LXB_CSS_VALUE_LARGER               = 0x0128,
    LXB_CSS_VALUE_SMALLER              = 0x0129,
    LXB_CSS_VALUE_NORMAL               = 0x012a,
    LXB_CSS_VALUE_ULTRA_CONDENSED      = 0x012b,
    LXB_CSS_VALUE_EXTRA_CONDENSED      = 0x012c,
    LXB_CSS_VALUE_CONDENSED            = 0x012d,
    LXB_CSS_VALUE_SEMI_CONDENSED       = 0x012e,
    LXB_CSS_VALUE_SEMI_EXPANDED        = 0x012f,
    LXB_CSS_VALUE_EXPANDED             = 0x0130,
    LXB_CSS_VALUE_EXTRA_EXPANDED       = 0x0131,
    LXB_CSS_VALUE_ULTRA_EXPANDED       = 0x0132,
    LXB_CSS_VALUE_ITALIC               = 0x0133,
    LXB_CSS_VALUE_OBLIQUE              = 0x0134,
    LXB_CSS_VALUE_BOLD                 = 0x0135,
    LXB_CSS_VALUE_BOLDER               = 0x0136,
    LXB_CSS_VALUE_LIGHTER              = 0x0137,
    LXB_CSS_VALUE_FORCE_END            = 0x0138,
    LXB_CSS_VALUE_ALLOW_END            = 0x0139,
    LXB_CSS_VALUE_MIN_CONTENT          = 0x013a,
    LXB_CSS_VALUE_MAX_CONTENT          = 0x013b,
    LXB_CSS_VALUE__ANGLE               = 0x013c,
    LXB_CSS_VALUE_MANUAL               = 0x013d,
    LXB_CSS_VALUE_LOOSE                = 0x013e,
    LXB_CSS_VALUE_STRICT               = 0x013f,
    LXB_CSS_VALUE_ANYWHERE             = 0x0140,
    LXB_CSS_VALUE_VISIBLE              = 0x0141,
    LXB_CSS_VALUE_CLIP                 = 0x0142,
    LXB_CSS_VALUE_SCROLL               = 0x0143,
    LXB_CSS_VALUE_BREAK_WORD           = 0x0144,
    LXB_CSS_VALUE_STATIC               = 0x0145,
    LXB_CSS_VALUE_RELATIVE             = 0x0146,
    LXB_CSS_VALUE_ABSOLUTE             = 0x0147,
    LXB_CSS_VALUE_STICKY               = 0x0148,
    LXB_CSS_VALUE_FIXED                = 0x0149,
    LXB_CSS_VALUE_JUSTIFY              = 0x014a,
    LXB_CSS_VALUE_MATCH_PARENT         = 0x014b,
    LXB_CSS_VALUE_JUSTIFY_ALL          = 0x014c,
    LXB_CSS_VALUE_ALL                  = 0x014d,
    LXB_CSS_VALUE_DIGITS               = 0x014e,
    LXB_CSS_VALUE_UNDERLINE            = 0x014f,
    LXB_CSS_VALUE_OVERLINE             = 0x0150,
    LXB_CSS_VALUE_LINE_THROUGH         = 0x0151,
    LXB_CSS_VALUE_BLINK                = 0x0152,
    LXB_CSS_VALUE_WAVY                 = 0x0153,
    LXB_CSS_VALUE_EACH_LINE            = 0x0154,
    LXB_CSS_VALUE_INTER_WORD           = 0x0155,
    LXB_CSS_VALUE_INTER_CHARACTER      = 0x0156,
    LXB_CSS_VALUE_MIXED                = 0x0157,
    LXB_CSS_VALUE_UPRIGHT              = 0x0158,
    LXB_CSS_VALUE_SIDEWAYS             = 0x0159,
    LXB_CSS_VALUE_ELLIPSIS             = 0x015a,
    LXB_CSS_VALUE_CAPITALIZE           = 0x015b,
    LXB_CSS_VALUE_UPPERCASE            = 0x015c,
    LXB_CSS_VALUE_LOWERCASE            = 0x015d,
    LXB_CSS_VALUE_FULL_WIDTH           = 0x015e,
    LXB_CSS_VALUE_FULL_SIZE_KANA       = 0x015f,
    LXB_CSS_VALUE_EMBED                = 0x0160,
    LXB_CSS_VALUE_ISOLATE              = 0x0161,
    LXB_CSS_VALUE_BIDI_OVERRIDE        = 0x0162,
    LXB_CSS_VALUE_ISOLATE_OVERRIDE     = 0x0163,
    LXB_CSS_VALUE_PLAINTEXT            = 0x0164,
    LXB_CSS_VALUE_COLLAPSE             = 0x0165,
    LXB_CSS_VALUE_PRE                  = 0x0166,
    LXB_CSS_VALUE_PRE_WRAP             = 0x0167,
    LXB_CSS_VALUE_BREAK_SPACES         = 0x0168,
    LXB_CSS_VALUE_PRE_LINE             = 0x0169,
    LXB_CSS_VALUE_KEEP_ALL             = 0x016a,
    LXB_CSS_VALUE_BREAK_ALL            = 0x016b,
    LXB_CSS_VALUE_BOTH                 = 0x016c,
    LXB_CSS_VALUE_MINIMUM              = 0x016d,
    LXB_CSS_VALUE_MAXIMUM              = 0x016e,
    LXB_CSS_VALUE_CLEAR                = 0x016f,
    LXB_CSS_VALUE_HORIZONTAL_TB        = 0x0170,
    LXB_CSS_VALUE_VERTICAL_RL          = 0x0171,
    LXB_CSS_VALUE_VERTICAL_LR          = 0x0172,
    LXB_CSS_VALUE_SIDEWAYS_RL          = 0x0173,
    LXB_CSS_VALUE_SIDEWAYS_LR          = 0x0174,
    LXB_CSS_VALUE__LAST_ENTRY          = 0x0175
};
typedef unsigned int lxb_css_value_type_t;


#endif /* LXB_CSS_VALUE_CONST_H */