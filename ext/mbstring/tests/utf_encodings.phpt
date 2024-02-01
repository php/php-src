--TEST--
Torture test for UTF-{7,8,16,32}
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(232); /* Make results consistent */
mb_substitute_character(0x25); // '%'
include('encoding_tests.inc');

// all ranges of valid codepoints in UnicodeData.txt
$validRanges = [
 [0x0, 0x377],
 [0x37a, 0x37f],
 [0x384, 0x38a],
 [0x38c, 0x38c],
 [0x38e, 0x3a1],
 [0x3a3, 0x52f],
 [0x531, 0x556],
 [0x559, 0x58a],
 [0x58d, 0x58f],
 [0x591, 0x5c7],
 [0x5d0, 0x5ea],
 [0x5ef, 0x5f4],
 [0x600, 0x61c],
 [0x61e, 0x70d],
 [0x70f, 0x74a],
 [0x74d, 0x7b1],
 [0x7c0, 0x7fa],
 [0x7fd, 0x82d],
 [0x830, 0x83e],
 [0x840, 0x85b],
 [0x85e, 0x85e],
 [0x860, 0x86a],
 [0x8a0, 0x8b4],
 [0x8b6, 0x8c7],
 [0x8d3, 0x983],
 [0x985, 0x98c],
 [0x98f, 0x990],
 [0x993, 0x9a8],
 [0x9aa, 0x9b0],
 [0x9b2, 0x9b2],
 [0x9b6, 0x9b9],
 [0x9bc, 0x9c4],
 [0x9c7, 0x9c8],
 [0x9cb, 0x9ce],
 [0x9d7, 0x9d7],
 [0x9dc, 0x9dd],
 [0x9df, 0x9e3],
 [0x9e6, 0x9fe],
 [0xa01, 0xa03],
 [0xa05, 0xa0a],
 [0xa0f, 0xa10],
 [0xa13, 0xa28],
 [0xa2a, 0xa30],
 [0xa32, 0xa33],
 [0xa35, 0xa36],
 [0xa38, 0xa39],
 [0xa3c, 0xa3c],
 [0xa3e, 0xa42],
 [0xa47, 0xa48],
 [0xa4b, 0xa4d],
 [0xa51, 0xa51],
 [0xa59, 0xa5c],
 [0xa5e, 0xa5e],
 [0xa66, 0xa76],
 [0xa81, 0xa83],
 [0xa85, 0xa8d],
 [0xa8f, 0xa91],
 [0xa93, 0xaa8],
 [0xaaa, 0xab0],
 [0xab2, 0xab3],
 [0xab5, 0xab9],
 [0xabc, 0xac5],
 [0xac7, 0xac9],
 [0xacb, 0xacd],
 [0xad0, 0xad0],
 [0xae0, 0xae3],
 [0xae6, 0xaf1],
 [0xaf9, 0xaff],
 [0xb01, 0xb03],
 [0xb05, 0xb0c],
 [0xb0f, 0xb10],
 [0xb13, 0xb28],
 [0xb2a, 0xb30],
 [0xb32, 0xb33],
 [0xb35, 0xb39],
 [0xb3c, 0xb44],
 [0xb47, 0xb48],
 [0xb4b, 0xb4d],
 [0xb55, 0xb57],
 [0xb5c, 0xb5d],
 [0xb5f, 0xb63],
 [0xb66, 0xb77],
 [0xb82, 0xb83],
 [0xb85, 0xb8a],
 [0xb8e, 0xb90],
 [0xb92, 0xb95],
 [0xb99, 0xb9a],
 [0xb9c, 0xb9c],
 [0xb9e, 0xb9f],
 [0xba3, 0xba4],
 [0xba8, 0xbaa],
 [0xbae, 0xbb9],
 [0xbbe, 0xbc2],
 [0xbc6, 0xbc8],
 [0xbca, 0xbcd],
 [0xbd0, 0xbd0],
 [0xbd7, 0xbd7],
 [0xbe6, 0xbfa],
 [0xc00, 0xc0c],
 [0xc0e, 0xc10],
 [0xc12, 0xc28],
 [0xc2a, 0xc39],
 [0xc3d, 0xc44],
 [0xc46, 0xc48],
 [0xc4a, 0xc4d],
 [0xc55, 0xc56],
 [0xc58, 0xc5a],
 [0xc60, 0xc63],
 [0xc66, 0xc6f],
 [0xc77, 0xc8c],
 [0xc8e, 0xc90],
 [0xc92, 0xca8],
 [0xcaa, 0xcb3],
 [0xcb5, 0xcb9],
 [0xcbc, 0xcc4],
 [0xcc6, 0xcc8],
 [0xcca, 0xccd],
 [0xcd5, 0xcd6],
 [0xcde, 0xcde],
 [0xce0, 0xce3],
 [0xce6, 0xcef],
 [0xcf1, 0xcf2],
 [0xd00, 0xd0c],
 [0xd0e, 0xd10],
 [0xd12, 0xd44],
 [0xd46, 0xd48],
 [0xd4a, 0xd4f],
 [0xd54, 0xd63],
 [0xd66, 0xd7f],
 [0xd81, 0xd83],
 [0xd85, 0xd96],
 [0xd9a, 0xdb1],
 [0xdb3, 0xdbb],
 [0xdbd, 0xdbd],
 [0xdc0, 0xdc6],
 [0xdca, 0xdca],
 [0xdcf, 0xdd4],
 [0xdd6, 0xdd6],
 [0xdd8, 0xddf],
 [0xde6, 0xdef],
 [0xdf2, 0xdf4],
 [0xe01, 0xe3a],
 [0xe3f, 0xe5b],
 [0xe81, 0xe82],
 [0xe84, 0xe84],
 [0xe86, 0xe8a],
 [0xe8c, 0xea3],
 [0xea5, 0xea5],
 [0xea7, 0xebd],
 [0xec0, 0xec4],
 [0xec6, 0xec6],
 [0xec8, 0xecd],
 [0xed0, 0xed9],
 [0xedc, 0xedf],
 [0xf00, 0xf47],
 [0xf49, 0xf6c],
 [0xf71, 0xf97],
 [0xf99, 0xfbc],
 [0xfbe, 0xfcc],
 [0xfce, 0xfda],
 [0x1000, 0x10c5],
 [0x10c7, 0x10c7],
 [0x10cd, 0x10cd],
 [0x10d0, 0x1248],
 [0x124a, 0x124d],
 [0x1250, 0x1256],
 [0x1258, 0x1258],
 [0x125a, 0x125d],
 [0x1260, 0x1288],
 [0x128a, 0x128d],
 [0x1290, 0x12b0],
 [0x12b2, 0x12b5],
 [0x12b8, 0x12be],
 [0x12c0, 0x12c0],
 [0x12c2, 0x12c5],
 [0x12c8, 0x12d6],
 [0x12d8, 0x1310],
 [0x1312, 0x1315],
 [0x1318, 0x135a],
 [0x135d, 0x137c],
 [0x1380, 0x1399],
 [0x13a0, 0x13f5],
 [0x13f8, 0x13fd],
 [0x1400, 0x169c],
 [0x16a0, 0x16f8],
 [0x1700, 0x170c],
 [0x170e, 0x1714],
 [0x1720, 0x1736],
 [0x1740, 0x1753],
 [0x1760, 0x176c],
 [0x176e, 0x1770],
 [0x1772, 0x1773],
 [0x1780, 0x17dd],
 [0x17e0, 0x17e9],
 [0x17f0, 0x17f9],
 [0x1800, 0x180e],
 [0x1810, 0x1819],
 [0x1820, 0x1878],
 [0x1880, 0x18aa],
 [0x18b0, 0x18f5],
 [0x1900, 0x191e],
 [0x1920, 0x192b],
 [0x1930, 0x193b],
 [0x1940, 0x1940],
 [0x1944, 0x196d],
 [0x1970, 0x1974],
 [0x1980, 0x19ab],
 [0x19b0, 0x19c9],
 [0x19d0, 0x19da],
 [0x19de, 0x1a1b],
 [0x1a1e, 0x1a5e],
 [0x1a60, 0x1a7c],
 [0x1a7f, 0x1a89],
 [0x1a90, 0x1a99],
 [0x1aa0, 0x1aad],
 [0x1ab0, 0x1ac0],
 [0x1b00, 0x1b4b],
 [0x1b50, 0x1b7c],
 [0x1b80, 0x1bf3],
 [0x1bfc, 0x1c37],
 [0x1c3b, 0x1c49],
 [0x1c4d, 0x1c88],
 [0x1c90, 0x1cba],
 [0x1cbd, 0x1cc7],
 [0x1cd0, 0x1cfa],
 [0x1d00, 0x1df9],
 [0x1dfb, 0x1f15],
 [0x1f18, 0x1f1d],
 [0x1f20, 0x1f45],
 [0x1f48, 0x1f4d],
 [0x1f50, 0x1f57],
 [0x1f59, 0x1f59],
 [0x1f5b, 0x1f5b],
 [0x1f5d, 0x1f5d],
 [0x1f5f, 0x1f7d],
 [0x1f80, 0x1fb4],
 [0x1fb6, 0x1fc4],
 [0x1fc6, 0x1fd3],
 [0x1fd6, 0x1fdb],
 [0x1fdd, 0x1fef],
 [0x1ff2, 0x1ff4],
 [0x1ff6, 0x1ffe],
 [0x2000, 0x2064],
 [0x2066, 0x2071],
 [0x2074, 0x208e],
 [0x2090, 0x209c],
 [0x20a0, 0x20bf],
 [0x20d0, 0x20f0],
 [0x2100, 0x218b],
 [0x2190, 0x2426],
 [0x2440, 0x244a],
 [0x2460, 0x2b73],
 [0x2b76, 0x2b95],
 [0x2b97, 0x2c2e],
 [0x2c30, 0x2c5e],
 [0x2c60, 0x2cf3],
 [0x2cf9, 0x2d25],
 [0x2d27, 0x2d27],
 [0x2d2d, 0x2d2d],
 [0x2d30, 0x2d67],
 [0x2d6f, 0x2d70],
 [0x2d7f, 0x2d96],
 [0x2da0, 0x2da6],
 [0x2da8, 0x2dae],
 [0x2db0, 0x2db6],
 [0x2db8, 0x2dbe],
 [0x2dc0, 0x2dc6],
 [0x2dc8, 0x2dce],
 [0x2dd0, 0x2dd6],
 [0x2dd8, 0x2dde],
 [0x2de0, 0x2e52],
 [0x2e80, 0x2e99],
 [0x2e9b, 0x2ef3],
 [0x2f00, 0x2fd5],
 [0x2ff0, 0x2ffb],
 [0x3000, 0x303f],
 [0x3041, 0x3096],
 [0x3099, 0x30ff],
 [0x3105, 0x312f],
 [0x3131, 0x318e],
 [0x3190, 0x31e3],
 [0x31f0, 0x321e],
 [0x3220, 0x3400],
 [0x4dbf, 0x4e00],
 [0x9ffc, 0x9ffc],
 [0xa000, 0xa48c],
 [0xa490, 0xa4c6],
 [0xa4d0, 0xa62b],
 [0xa640, 0xa6f7],
 [0xa700, 0xa7bf],
 [0xa7c2, 0xa7ca],
 [0xa7f5, 0xa82c],
 [0xa830, 0xa839],
 [0xa840, 0xa877],
 [0xa880, 0xa8c5],
 [0xa8ce, 0xa8d9],
 [0xa8e0, 0xa953],
 [0xa95f, 0xa97c],
 [0xa980, 0xa9cd],
 [0xa9cf, 0xa9d9],
 [0xa9de, 0xa9fe],
 [0xaa00, 0xaa36],
 [0xaa40, 0xaa4d],
 [0xaa50, 0xaa59],
 [0xaa5c, 0xaac2],
 [0xaadb, 0xaaf6],
 [0xab01, 0xab06],
 [0xab09, 0xab0e],
 [0xab11, 0xab16],
 [0xab20, 0xab26],
 [0xab28, 0xab2e],
 [0xab30, 0xab6b],
 [0xab70, 0xabed],
 [0xabf0, 0xabf9],
 [0xac00, 0xac00],
 [0xd7a3, 0xd7a3],
 [0xd7b0, 0xd7c6],
 [0xd7cb, 0xd7fb],
 [0xd800, 0xd800],
 [0xdb7f, 0xdb80],
 [0xdbff, 0xdc00],
 [0xdfff, 0xe000],
 [0xf8ff, 0xfa6d],
 [0xfa70, 0xfad9],
 [0xfb00, 0xfb06],
 [0xfb13, 0xfb17],
 [0xfb1d, 0xfb36],
 [0xfb38, 0xfb3c],
 [0xfb3e, 0xfb3e],
 [0xfb40, 0xfb41],
 [0xfb43, 0xfb44],
 [0xfb46, 0xfbc1],
 [0xfbd3, 0xfd3f],
 [0xfd50, 0xfd8f],
 [0xfd92, 0xfdc7],
 [0xfdf0, 0xfdfd],
 [0xfe00, 0xfe19],
 [0xfe20, 0xfe52],
 [0xfe54, 0xfe66],
 [0xfe68, 0xfe6b],
 [0xfe70, 0xfe74],
 [0xfe76, 0xfefc],
 [0xfeff, 0xfeff],
 [0xff01, 0xffbe],
 [0xffc2, 0xffc7],
 [0xffca, 0xffcf],
 [0xffd2, 0xffd7],
 [0xffda, 0xffdc],
 [0xffe0, 0xffe6],
 [0xffe8, 0xffee],
 [0xfff9, 0xfffd],
 [0x10000, 0x1000b],
 [0x1000d, 0x10026],
 [0x10028, 0x1003a],
 [0x1003c, 0x1003d],
 [0x1003f, 0x1004d],
 [0x10050, 0x1005d],
 [0x10080, 0x100fa],
 [0x10100, 0x10102],
 [0x10107, 0x10133],
 [0x10137, 0x1018e],
 [0x10190, 0x1019c],
 [0x101a0, 0x101a0],
 [0x101d0, 0x101fd],
 [0x10280, 0x1029c],
 [0x102a0, 0x102d0],
 [0x102e0, 0x102fb],
 [0x10300, 0x10323],
 [0x1032d, 0x1034a],
 [0x10350, 0x1037a],
 [0x10380, 0x1039d],
 [0x1039f, 0x103c3],
 [0x103c8, 0x103d5],
 [0x10400, 0x1049d],
 [0x104a0, 0x104a9],
 [0x104b0, 0x104d3],
 [0x104d8, 0x104fb],
 [0x10500, 0x10527],
 [0x10530, 0x10563],
 [0x1056f, 0x1056f],
 [0x10600, 0x10736],
 [0x10740, 0x10755],
 [0x10760, 0x10767],
 [0x10800, 0x10805],
 [0x10808, 0x10808],
 [0x1080a, 0x10835],
 [0x10837, 0x10838],
 [0x1083c, 0x1083c],
 [0x1083f, 0x10855],
 [0x10857, 0x1089e],
 [0x108a7, 0x108af],
 [0x108e0, 0x108f2],
 [0x108f4, 0x108f5],
 [0x108fb, 0x1091b],
 [0x1091f, 0x10939],
 [0x1093f, 0x1093f],
 [0x10980, 0x109b7],
 [0x109bc, 0x109cf],
 [0x109d2, 0x10a03],
 [0x10a05, 0x10a06],
 [0x10a0c, 0x10a13],
 [0x10a15, 0x10a17],
 [0x10a19, 0x10a35],
 [0x10a38, 0x10a3a],
 [0x10a3f, 0x10a48],
 [0x10a50, 0x10a58],
 [0x10a60, 0x10a9f],
 [0x10ac0, 0x10ae6],
 [0x10aeb, 0x10af6],
 [0x10b00, 0x10b35],
 [0x10b39, 0x10b55],
 [0x10b58, 0x10b72],
 [0x10b78, 0x10b91],
 [0x10b99, 0x10b9c],
 [0x10ba9, 0x10baf],
 [0x10c00, 0x10c48],
 [0x10c80, 0x10cb2],
 [0x10cc0, 0x10cf2],
 [0x10cfa, 0x10d27],
 [0x10d30, 0x10d39],
 [0x10e60, 0x10e7e],
 [0x10e80, 0x10ea9],
 [0x10eab, 0x10ead],
 [0x10eb0, 0x10eb1],
 [0x10f00, 0x10f27],
 [0x10f30, 0x10f59],
 [0x10fb0, 0x10fcb],
 [0x10fe0, 0x10ff6],
 [0x11000, 0x1104d],
 [0x11052, 0x1106f],
 [0x1107f, 0x110c1],
 [0x110cd, 0x110cd],
 [0x110d0, 0x110e8],
 [0x110f0, 0x110f9],
 [0x11100, 0x11134],
 [0x11136, 0x11147],
 [0x11150, 0x11176],
 [0x11180, 0x111df],
 [0x111e1, 0x111f4],
 [0x11200, 0x11211],
 [0x11213, 0x1123e],
 [0x11280, 0x11286],
 [0x11288, 0x11288],
 [0x1128a, 0x1128d],
 [0x1128f, 0x1129d],
 [0x1129f, 0x112a9],
 [0x112b0, 0x112ea],
 [0x112f0, 0x112f9],
 [0x11300, 0x11303],
 [0x11305, 0x1130c],
 [0x1130f, 0x11310],
 [0x11313, 0x11328],
 [0x1132a, 0x11330],
 [0x11332, 0x11333],
 [0x11335, 0x11339],
 [0x1133b, 0x11344],
 [0x11347, 0x11348],
 [0x1134b, 0x1134d],
 [0x11350, 0x11350],
 [0x11357, 0x11357],
 [0x1135d, 0x11363],
 [0x11366, 0x1136c],
 [0x11370, 0x11374],
 [0x11400, 0x1145b],
 [0x1145d, 0x11461],
 [0x11480, 0x114c7],
 [0x114d0, 0x114d9],
 [0x11580, 0x115b5],
 [0x115b8, 0x115dd],
 [0x11600, 0x11644],
 [0x11650, 0x11659],
 [0x11660, 0x1166c],
 [0x11680, 0x116b8],
 [0x116c0, 0x116c9],
 [0x11700, 0x1171a],
 [0x1171d, 0x1172b],
 [0x11730, 0x1173f],
 [0x11800, 0x1183b],
 [0x118a0, 0x118f2],
 [0x118ff, 0x11906],
 [0x11909, 0x11909],
 [0x1190c, 0x11913],
 [0x11915, 0x11916],
 [0x11918, 0x11935],
 [0x11937, 0x11938],
 [0x1193b, 0x11946],
 [0x11950, 0x11959],
 [0x119a0, 0x119a7],
 [0x119aa, 0x119d7],
 [0x119da, 0x119e4],
 [0x11a00, 0x11a47],
 [0x11a50, 0x11aa2],
 [0x11ac0, 0x11af8],
 [0x11c00, 0x11c08],
 [0x11c0a, 0x11c36],
 [0x11c38, 0x11c45],
 [0x11c50, 0x11c6c],
 [0x11c70, 0x11c8f],
 [0x11c92, 0x11ca7],
 [0x11ca9, 0x11cb6],
 [0x11d00, 0x11d06],
 [0x11d08, 0x11d09],
 [0x11d0b, 0x11d36],
 [0x11d3a, 0x11d3a],
 [0x11d3c, 0x11d3d],
 [0x11d3f, 0x11d47],
 [0x11d50, 0x11d59],
 [0x11d60, 0x11d65],
 [0x11d67, 0x11d68],
 [0x11d6a, 0x11d8e],
 [0x11d90, 0x11d91],
 [0x11d93, 0x11d98],
 [0x11da0, 0x11da9],
 [0x11ee0, 0x11ef8],
 [0x11fb0, 0x11fb0],
 [0x11fc0, 0x11ff1],
 [0x11fff, 0x12399],
 [0x12400, 0x1246e],
 [0x12470, 0x12474],
 [0x12480, 0x12543],
 [0x13000, 0x1342e],
 [0x13430, 0x13438],
 [0x14400, 0x14646],
 [0x16800, 0x16a38],
 [0x16a40, 0x16a5e],
 [0x16a60, 0x16a69],
 [0x16a6e, 0x16a6f],
 [0x16ad0, 0x16aed],
 [0x16af0, 0x16af5],
 [0x16b00, 0x16b45],
 [0x16b50, 0x16b59],
 [0x16b5b, 0x16b61],
 [0x16b63, 0x16b77],
 [0x16b7d, 0x16b8f],
 [0x16e40, 0x16e9a],
 [0x16f00, 0x16f4a],
 [0x16f4f, 0x16f87],
 [0x16f8f, 0x16f9f],
 [0x16fe0, 0x16fe4],
 [0x16ff0, 0x16ff1],
 [0x17000, 0x17000],
 [0x187f7, 0x187f7],
 [0x18800, 0x18cd5],
 [0x18d00, 0x18d00],
 [0x18d08, 0x18d08],
 [0x1b000, 0x1b11e],
 [0x1b150, 0x1b152],
 [0x1b164, 0x1b167],
 [0x1b170, 0x1b2fb],
 [0x1bc00, 0x1bc6a],
 [0x1bc70, 0x1bc7c],
 [0x1bc80, 0x1bc88],
 [0x1bc90, 0x1bc99],
 [0x1bc9c, 0x1bca3],
 [0x1d000, 0x1d0f5],
 [0x1d100, 0x1d126],
 [0x1d129, 0x1d1e8],
 [0x1d200, 0x1d245],
 [0x1d2e0, 0x1d2f3],
 [0x1d300, 0x1d356],
 [0x1d360, 0x1d378],
 [0x1d400, 0x1d454],
 [0x1d456, 0x1d49c],
 [0x1d49e, 0x1d49f],
 [0x1d4a2, 0x1d4a2],
 [0x1d4a5, 0x1d4a6],
 [0x1d4a9, 0x1d4ac],
 [0x1d4ae, 0x1d4b9],
 [0x1d4bb, 0x1d4bb],
 [0x1d4bd, 0x1d4c3],
 [0x1d4c5, 0x1d505],
 [0x1d507, 0x1d50a],
 [0x1d50d, 0x1d514],
 [0x1d516, 0x1d51c],
 [0x1d51e, 0x1d539],
 [0x1d53b, 0x1d53e],
 [0x1d540, 0x1d544],
 [0x1d546, 0x1d546],
 [0x1d54a, 0x1d550],
 [0x1d552, 0x1d6a5],
 [0x1d6a8, 0x1d7cb],
 [0x1d7ce, 0x1da8b],
 [0x1da9b, 0x1da9f],
 [0x1daa1, 0x1daaf],
 [0x1e000, 0x1e006],
 [0x1e008, 0x1e018],
 [0x1e01b, 0x1e021],
 [0x1e023, 0x1e024],
 [0x1e026, 0x1e02a],
 [0x1e100, 0x1e12c],
 [0x1e130, 0x1e13d],
 [0x1e140, 0x1e149],
 [0x1e14e, 0x1e14f],
 [0x1e2c0, 0x1e2f9],
 [0x1e2ff, 0x1e2ff],
 [0x1e800, 0x1e8c4],
 [0x1e8c7, 0x1e8d6],
 [0x1e900, 0x1e94b],
 [0x1e950, 0x1e959],
 [0x1e95e, 0x1e95f],
 [0x1ec71, 0x1ecb4],
 [0x1ed01, 0x1ed3d],
 [0x1ee00, 0x1ee03],
 [0x1ee05, 0x1ee1f],
 [0x1ee21, 0x1ee22],
 [0x1ee24, 0x1ee24],
 [0x1ee27, 0x1ee27],
 [0x1ee29, 0x1ee32],
 [0x1ee34, 0x1ee37],
 [0x1ee39, 0x1ee39],
 [0x1ee3b, 0x1ee3b],
 [0x1ee42, 0x1ee42],
 [0x1ee47, 0x1ee47],
 [0x1ee49, 0x1ee49],
 [0x1ee4b, 0x1ee4b],
 [0x1ee4d, 0x1ee4f],
 [0x1ee51, 0x1ee52],
 [0x1ee54, 0x1ee54],
 [0x1ee57, 0x1ee57],
 [0x1ee59, 0x1ee59],
 [0x1ee5b, 0x1ee5b],
 [0x1ee5d, 0x1ee5d],
 [0x1ee5f, 0x1ee5f],
 [0x1ee61, 0x1ee62],
 [0x1ee64, 0x1ee64],
 [0x1ee67, 0x1ee6a],
 [0x1ee6c, 0x1ee72],
 [0x1ee74, 0x1ee77],
 [0x1ee79, 0x1ee7c],
 [0x1ee7e, 0x1ee7e],
 [0x1ee80, 0x1ee89],
 [0x1ee8b, 0x1ee9b],
 [0x1eea1, 0x1eea3],
 [0x1eea5, 0x1eea9],
 [0x1eeab, 0x1eebb],
 [0x1eef0, 0x1eef1],
 [0x1f000, 0x1f02b],
 [0x1f030, 0x1f093],
 [0x1f0a0, 0x1f0ae],
 [0x1f0b1, 0x1f0bf],
 [0x1f0c1, 0x1f0cf],
 [0x1f0d1, 0x1f0f5],
 [0x1f100, 0x1f1ad],
 [0x1f1e6, 0x1f202],
 [0x1f210, 0x1f23b],
 [0x1f240, 0x1f248],
 [0x1f250, 0x1f251],
 [0x1f260, 0x1f265],
 [0x1f300, 0x1f6d7],
 [0x1f6e0, 0x1f6ec],
 [0x1f6f0, 0x1f6fc],
 [0x1f700, 0x1f773],
 [0x1f780, 0x1f7d8],
 [0x1f7e0, 0x1f7eb],
 [0x1f800, 0x1f80b],
 [0x1f810, 0x1f847],
 [0x1f850, 0x1f859],
 [0x1f860, 0x1f887],
 [0x1f890, 0x1f8ad],
 [0x1f8b0, 0x1f8b1],
 [0x1f900, 0x1f978],
 [0x1f97a, 0x1f9cb],
 [0x1f9cd, 0x1fa53],
 [0x1fa60, 0x1fa6d],
 [0x1fa70, 0x1fa74],
 [0x1fa78, 0x1fa7a],
 [0x1fa80, 0x1fa86],
 [0x1fa90, 0x1faa8],
 [0x1fab0, 0x1fab6],
 [0x1fac0, 0x1fac2],
 [0x1fad0, 0x1fad6],
 [0x1fb00, 0x1fb92],
 [0x1fb94, 0x1fbca],
 [0x1fbf0, 0x1fbf9],
 [0x20000, 0x20000],
 [0x2a6dd, 0x2a6dd],
 [0x2a700, 0x2a700],
 [0x2b734, 0x2b734],
 [0x2b740, 0x2b740],
 [0x2b81d, 0x2b81d],
 [0x2b820, 0x2b820],
 [0x2cea1, 0x2cea1],
 [0x2ceb0, 0x2ceb0],
 [0x2ebe0, 0x2ebe0],
 [0x2f800, 0x2fa1d],
 [0x30000, 0x30000],
 [0x3134a, 0x3134a],
 [0xe0001, 0xe0001],
 [0xe0020, 0xe007f],
 [0xe0100, 0xe01ef],
 [0xf0000, 0xf0000],
 [0xffffd, 0xffffd],
 [0x100000, 0x100000],
 [0x10fffd, 0x10fffd]];

// in UTF-32BE
$validCodepoints = array();

foreach ($validRanges as $range) {
  for ($cp = $range[0]; $cp <= $range[1]; $cp++) {
    if (($cp < 0xD800 || $cp > 0xDFFF) && $cp !== 0xFEFF)
      $validCodepoints[pack('N', $cp)] = true;
  }
}

function testValidCodepoints($encoding) {
  global $validCodepoints;

  $good = array_keys($validCodepoints);
  shuffle($good);

  while (!empty($good)) {
    $string = '';
    $length = min(rand(20,30), count($good));
    while ($length--) {
      $string .= array_pop($good);
    }

    $converted = mb_convert_encoding($string, $encoding, 'UTF-32BE');
    if ($converted === false)
      die("mb_convert_encoding failed to convert UTF-32BE to $encoding." .
          "\nString: " . bin2hex($string));
    testValidString($converted, $string, $encoding, 'UTF-32BE');
  }
}

function testInvalidCodepoints($invalid, $encoding) {
  global $validCodepoints;

  $good = array_keys($validCodepoints);
  shuffle($good);

  foreach ($invalid as $bad => $expected) {
    $good1  = array_pop($good);
    $string =  $bad . mb_convert_encoding($good1, $encoding, 'UTF-32BE');
    testInvalidString($string, $expected . $good1, $encoding, 'UTF-32BE');
  }
}

echo "== UTF-8 ==\n";

testValidCodepoints('UTF-8');

testValidString('', '', 'UTF-8', 'UTF-32BE');

$invalid = array(
  // Codepoints outside of valid 0-0x10FFFF range for Unicode
  "\xF4\x90\x80\x80" => str_repeat("\x00\x00\x00%", 4), // CP 0x110000
  "\xF7\x80\x80\x80" => str_repeat("\x00\x00\x00%", 4), // CP 0x1C0000
  "\xF7\xBF\xBF\xBF" => str_repeat("\x00\x00\x00%", 4), // CP 0x1FFFFF

  // Reserved range for UTF-16 surrogate pairs
  "\xED\xA0\x80" => str_repeat("\x00\x00\x00%", 3),     // CP 0xD800
  "\xED\xAF\xBF" => str_repeat("\x00\x00\x00%", 3),     // CP 0xDBFF
  "\xED\xBF\xBF" => str_repeat("\x00\x00\x00%", 3),     // CP 0xDFFF

  // Truncated characters
  "\xDF" => "\x00\x00\x00%",         // should have been 2-byte
  "\xEF\xBF" => "\x00\x00\x00%",     // should have been 3-byte
  "\xF0\xBF\xBF" => "\x00\x00\x00%", // should have been 4-byte
  "\xF1\x96" => "\x00\x00\x00%",
  "\xF1\x96\x80" => "\x00\x00\x00%",
  "\xF2\x94" => "\x00\x00\x00%",
  "\xF2\x94\x80" => "\x00\x00\x00%",
  "\xF3\x94" => "\x00\x00\x00%",
  "\xF3\x94\x80" => "\x00\x00\x00%",
  "\xE0\x9F" => "\x00\x00\x00%\x00\x00\x00%",
  "\xED\xA6" => "\x00\x00\x00%\x00\x00\x00%",

  // Multi-byte characters which end too soon and go to ASCII
  "\xDFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xEF\xBFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xF0\xBFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xF0\xBF\xBFA" => "\x00\x00\x00%\x00\x00\x00A",

  // Multi-byte characters which end too soon and go to another MB char
  "\xDF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",
  "\xEF\xBF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",
  "\xF0\xBF\xBF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",

  // Multi-byte characters which end too soon and go to a junk byte
  // (Which isn't even valid to start a new character)
  "\xF0\xBF\xBF\xFF" => str_repeat("\x00\x00\x00%", 2),
  "\xF0\xBF\xFF" => str_repeat("\x00\x00\x00%", 2),

  // Continuation bytes which appear outside of a MB char
  "\x80" => "\x00\x00\x00%",
  "A\x80" => "\x00\x00\x00A\x00\x00\x00%",
  "\xDF\xBF\x80" => "\x00\x00\x07\xFF\x00\x00\x00%",

  // Overlong code units
  // (Using more bytes than needed to encode a character)
  "\xC1\xBF" => str_repeat("\x00\x00\x00%", 2),        // didn't need 2 bytes
  "\xE0\x9F\xBF" => str_repeat("\x00\x00\x00%", 3),    // didn't need 3 bytes
  "\xF0\x8F\xBF\xBF" => str_repeat("\x00\x00\x00%", 4) // didn't need 4 bytes
);

testInvalidCodepoints($invalid, 'UTF-8');

echo "== UTF-16 ==\n";

testValidCodepoints("UTF-16");
testValidCodepoints("UTF-16LE");
testValidCodepoints("UTF-16BE");

testValidString('', '', 'UTF-16', 'UTF-32BE');
testValidString('', '', 'UTF-16LE', 'UTF-32BE');
testValidString('', '', 'UTF-16BE', 'UTF-32BE');

$invalid = array(
  // UTF-16 _cannot_ represent codepoints bigger than 0x10FFFF, so we're not
  // worried about that. But there are plenty of other ways to mess up...

  // Second half of surrogate pair comes first
  "\xDC\x01\xD8\x02" => "\x00\x00\x00%\x00\x00\x00%",

  // First half of surrogate pair not followed by second part
  "\xD8\x01\x00A" => "\x00\x00\x00%\x00\x00\x00A",

  // First half of surrogate pair at end of string
  "\xD8\x01" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-16');
testInvalidCodepoints($invalid, 'UTF-16BE');

// Truncated strings
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16', 'UTF-32BE');
testInvalidString("\x00A\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16', 'UTF-32BE');
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16BE', 'UTF-32BE');
testInvalidString("\x00A\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16BE', 'UTF-32BE');

$invalid = array(
  // Second half of surrogate pair comes first
  "\x01\xDC\x02\xD8" => "\x00\x00\x00%\x00\x00\x00%",

  // First half of surrogate pair not followed by second part
  "\x01\xD8A\x00" => "\x00\x00\x00%\x00\x00\x00A",

  // First half of surrogate pair at end of string
  "\x01\xD8" => "\x00\x00\x00%",

  // Two successive codepoints which are both the 1st part of a surrogate pair
  "\x01\xD8\x02\xD8" => "\x00\x00\x00%\x00\x00\x00%"
);

testInvalidCodepoints($invalid, 'UTF-16LE');

// Truncated
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16LE', 'UTF-32BE');
testInvalidString("A\x00\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16LE', 'UTF-32BE');

// Test treatment of BOM
testValidString("\xFE\xFF\x12\x34", "\x00\x00\x12\x34", 'UTF-16', 'UTF-32BE', false);
testValidString("\xFF\xFE\x12\x34", "\x00\x00\x34\x12", 'UTF-16', 'UTF-32BE', false);

// Test treatment of (illegal) codepoints between U+D800 and U+DFFF
testValidString("\xD8\x00", "\xD8\x00", 'UCS-2BE', 'UTF-16BE', false);
testValidString("\xDB\xFF", "\xDB\xFF", 'UCS-2BE', 'UTF-16BE', false);
testValidString("\xDC\x00", "\xDC\x00", 'UCS-2BE', 'UTF-16BE', false);
testValidString("\xD8\x00", "\x00\xD8", 'UCS-2BE', 'UTF-16LE', false);
testValidString("\xDC\x00", "\x00\xDC", 'UCS-2BE', 'UTF-16LE', false);

// Try codepoint over U+10FFFF
convertInvalidString("\x00\x11\x56\x78", "\x00%", 'UCS-4BE', 'UTF-16BE');
convertInvalidString("\x00\x11\x56\x78", "%\x00", 'UCS-4BE', 'UTF-16LE');

echo "== UTF-32 ==\n";

testValidCodepoints("UTF-32LE");
testValidCodepoints("UTF-32BE");

// Empty string
testValidString('', '', 'UTF-32', 'UTF-32BE');
testValidString('', '', 'UTF-32BE', 'UTF-32');
testValidString('', '', 'UTF-32LE', 'UTF-32BE');

$invalid = array(
  // Codepoints which are too big
  "\x00\x11\x00\x00" => "\x00\x00\x00%",
  "\x80\x00\x00\x00" => "\x00\x00\x00%",
  "\xff\xff\xfe\xff" => "\x00\x00\x00%",

  // Surrogates
  "\x00\x00\xd8\x00" => "\x00\x00\x00%",
  "\x00\x00\xdb\xff" => "\x00\x00\x00%",
  "\x00\x00\xdc\x00" => "\x00\x00\x00%",
  "\x00\x00\xdf\xff" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-32');
testInvalidCodepoints($invalid, 'UTF-32BE');

// Truncated code units
testInvalidString("\x00\x01\x01", "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00\x01",     "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');

$invalid = array(
  // Codepoints which are too big
  "\x00\x00\x11\x00" => "\x00\x00\x00%",
  "\x00\x00\x00\x80" => "\x00\x00\x00%",
  "\xff\xfe\xff\xff" => "\x00\x00\x00%",

  // Surrogates
  "\x00\xd8\x00\x00" => "\x00\x00\x00%",
  "\xff\xdb\x00\x00" => "\x00\x00\x00%",
  "\x00\xdc\x00\x00" => "\x00\x00\x00%",
  "\xff\xdf\x00\x00" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-32LE');

// Truncated code units
testInvalidString("\x00\x01\x01", "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');
testInvalidString("\x00\x01",     "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');

// Test treatment of BOM
testValidString("\x00\x00\xFE\xFF\x00\x00\x12\x34", "\x00\x00\x12\x34", 'UTF-32', 'UTF-32BE', false);
testValidString("\xFF\xFE\x00\x00\x12\x34\x00\x00", "\x00\x00\x34\x12", 'UTF-32', 'UTF-32BE', false);

// Test treatment of (illegal) codepoints between U+D800 and U+DFFF
testValidString("\xD8\x00", "\x00\x00\xD8\x00", 'UCS-2BE', 'UTF-32BE', false);
testValidString("\xDB\xFF", "\x00\x00\xDB\xFF", 'UCS-2BE', 'UTF-32BE', false);
testValidString("\xDC\x00", "\x00\x00\xDC\x00", 'UCS-2BE', 'UTF-32BE', false);
testValidString("\xD8\x00", "\x00\xD8\x00\x00", 'UCS-2BE', 'UTF-32LE', false);
testValidString("\xDC\x00", "\x00\xDC\x00\x00", 'UCS-2BE', 'UTF-32LE', false);

echo "== UTF-7 ==\n";

testValidString('', '', 'UTF-7', 'UTF-32BE');

// 'Direct' characters
foreach (range(ord('A'), ord('Z')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (range(ord('a'), ord('z')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (range(ord('0'), ord('9')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (str_split("'(),-./:?") as $char)
  testValidString($char, "\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE');

// 'Optional direct' characters are Base64-encoded in mbstring's implementation

// Whitespace
foreach (str_split(" \t\r\n\x00") as $char)
  testValidString($char, "\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE');

// Encoding + as +-
testValidString('+-', "\x00\x00\x00+", 'UTF-7', 'UTF-32BE', false);

// UTF-16 + Base64 encoding
function encode($str, $encoding) {
  // Base64 encoding for UTF-7 doesn't use '=' for padding
  return str_replace('=', '', base64_encode(mb_convert_encoding($str, 'UTF-16BE', $encoding)));
}

for ($i = 0; $i < 256; $i++) {
  $reversible = true;
  if ($i >= ord('A') && $i <= ord('Z'))
    $reversible = false;
  if ($i >= ord('a') && $i <= ord('z'))
    $reversible = false;
  if ($i >= ord('0') && $i <= ord('9'))
    $reversible = false;
  if (strpos("'(),-./:?\x00 \t\r\n", chr($i)) !== false)
    $reversible = false;

  testValidString('+' . encode("\x00" . chr($i), 'UTF-16BE') . '-', "\x00\x00\x00" . chr($i), 'UTF-7', 'UTF-32BE', $reversible);
}

testValidString('+' . encode("\x12\x34", 'UTF-16BE') . '-', "\x00\x00\x12\x34", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x12\x34\x56\x78", 'UTF-16BE') . '-', "\x00\x00\x12\x34\x00\x00\x56\x78", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x12\x34\x56\x78\x00\x40", 'UTF-16BE') . '-', "\x00\x00\x12\x34\x00\x00\x56\x78\x00\x00\x00\x40", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\xFF\xEE\xEE\xFF", 'UTF-16BE') . '-', "\x00\x00\xFF\xEE\x00\x00\xEE\xFF", 'UTF-7', 'UTF-32BE');

// Surrogate pair
testValidString('+' . encode("\x00\x01\x04\x00", 'UTF-32BE') . '-', "\x00\x01\x04\x00", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x00\x00\x00A\x00\x01\x04\x00\x00\x00\x00B", 'UTF-32BE') . '-', "\x00\x00\x00A\x00\x01\x04\x00\x00\x00\x00B", 'UTF-7', 'UTF-32BE', false);
testValidString('+' . encode("\x00\x01\x04\x00\x00\x01\x04\x00", 'UTF-32BE') . '-', "\x00\x01\x04\x00\x00\x01\x04\x00", 'UTF-7', 'UTF-32BE');

// Unterminated + section
// (This is not considered illegal)
testValidString('+' . encode('ABC', 'ASCII'), "\x00A\x00B\x00C", 'UTF-7', 'UTF-16BE', false);

// + sections immediately after each other
// (This isn't illegal either)
testValidString('+' . encode('AB', 'ASCII') . '-+' . encode('CD', 'ASCII') . '-', "\x00A\x00B\x00C\x00D", 'UTF-7', 'UTF-16BE', false);

// + sections not immediately after each other
// (Just trying to be exhaustive here)
testValidString('+' . encode('AB', 'ASCII') . '-!+' . encode('CD', 'ASCII') . '-', "\x00A\x00B\x00!\x00C\x00D", 'UTF-7', 'UTF-16BE', false);

// + section terminated by a non-Base64 direct character which is NOT -
foreach (str_split(" \t\r\n'(),.:?!\"#$%&*;<=>@[]^_`{|}\x00") as $char) {
  testValidString('+' . encode("\x12\x34", 'UTF-16BE') . $char, "\x00\x00\x12\x34\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE', false);
}

// Non-direct character followed by direct character
testValidString('%A', '+ACU-A', 'ASCII', 'UTF-7');
testValidString('%%A', '+ACUAJQ-A', 'ASCII', 'UTF-7');
testValidString('%%%A', '+ACUAJQAl-A', 'ASCII', 'UTF-7');

// Now let's see how UTF-7 can go BAD...

function rawEncode($str) {
  return str_replace('=', '', base64_encode($str));
}

// Totally bogus byte
testInvalidString("\xFF", "%", 'UTF-7', 'UTF-8');
// Totally bogus codepoint... '+ACU-' is '%' in UTF-7'
testInvalidString("\x12\x34\x56\x78", "+ACU-", 'UTF-32BE', 'UTF-7');

// First, messed up UTF16 in + section
// Second half of surrogate pair coming first
testInvalidString('+' . rawEncode("\xDC\x01\xD8\x02") . '-', "\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\xDC\x01\xD8\x02") . '-', "\x00\x00\x00.\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\x00.\xDC\x01\xD8\x02") . '-', "\x00\x00\x00.\x00\x00\x00.\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// First half of surrogate pair not followed by second half
testInvalidString('+' . rawEncode("\xD8\x01\x00A") . '-', "\x00\x00\x00%\x00\x00\x00A", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\xD8\x01\xD9\x02") . '-', "\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\xD8\x01\x00A") . '-', "\x00\x00\x00.\x00\x00\x00%\x00\x00\x00A", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\xD8\x01\xD9\x02") . '-', "\x00\x00\x00.\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\x00.\xD8\x01\x00A") . '-', "\x00\x00\x00.\x00\x00\x00.\x00\x00\x00%\x00\x00\x00A", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\x00.\x00.\xD8\x01\xD9\x02") . '-', "\x00\x00\x00.\x00\x00\x00.\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// First half of surrogate pair appearing at end of string
testInvalidString('+' . rawEncode("\xD8\x01") . '-', "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\xD8\x01"), "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// Truncated string
testInvalidString('+' . rawEncode("\x01") . '-', "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');
testInvalidString('+l', "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// Base64 section should not have 4 ASCII characters; the first 3 can encode one
// UTF-16 character, so there is no need for the 4th
testInvalidString('+RR8I', "\xE4\x94\x9F%", 'UTF-7', 'UTF-8');
// Likewise with 7 characters
testInvalidString('+RR8IAAA', "\xE4\x94\x9F\xE0\xA0\x80%", 'UTF-7', 'UTF-8');

// Similarly, it is useless for a Base64 section to only contain a single 'A'
// (which decodes to only zero bits)
testInvalidString("+A", "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// And then, messed up Base64 encoding

// Bad padding on + section (not zeroes)
$encoded = encode("\x12\x34", 'UTF-16BE'); // 3 Base64 bytes, 2 bits of padding...
$corrupted = substr($encoded, 0, 2) . chr(ord($encoded[2]) + 1);
testInvalidString('+' . $corrupted . '-', "\x00\x00\x12\x34\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// Characters which are not Base64 (and not even ASCII) appearing in Base64 section
testInvalidString("+\x80", "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// Try codepoint over U+10FFFF; '+ACU-' is the error marker '%'
convertInvalidString("\x12\x34\x56\x78", "+ACU-", 'UCS-4BE', 'UTF-7');
convertInvalidString("\x00\x11\x56\x78", "+ACU-", 'UCS-4BE', 'UTF-7');

// If error marker character needs to be ASCII-encoded but is able to serve as an
// ending character for a Base64 section, no need to add an additional dash
mb_substitute_character(0x3F); // ?
convertInvalidString("\x1E\xBE", '+AB4?', 'UTF-7', 'UTF-7');

echo "Done!\n";

?>
--EXPECT--
== UTF-8 ==
== UTF-16 ==
== UTF-32 ==
== UTF-7 ==
Done!
