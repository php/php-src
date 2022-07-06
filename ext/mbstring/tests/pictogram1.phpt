--TEST--
Emoji (Pictogram characters for cellular phone in Japan) test based on Unicode 6.0
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_substitute_character(0x3f);

echo "SJIS-Mobile to Unicode\n";
var_dump(bin2hex(mb_convert_encoding("\xf8\x9f", "UCS-4BE", "SJIS-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xf6\x60", "UCS-4BE", "SJIS-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xf9\x8b", "UCS-4BE", "SJIS-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xf9\xe6", "UCS-4BE", "SJIS-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xf6\xec", "UCS-4BE", "SJIS-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xf7\x50", "UCS-4BE", "SJIS-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xf9\x85", "UCS-4BE", "SJIS-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xf4\x89", "UCS-4BE", "SJIS-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xf7\xb0", "UCS-4BE", "SJIS-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xf3\xd2", "UCS-4BE", "SJIS-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xfb\xb3", "UCS-4BE", "SJIS-Mobile#SOFTBANK")));

echo "Unicode to SJIS-Mobile\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "SJIS-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "SJIS-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "SJIS-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "SJIS-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "SJIS-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "SJIS-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "SJIS-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "SJIS-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "SJIS-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "SJIS-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "SJIS-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "SJIS-Mobile#SOFTBANK", "UCS-4BE")));

echo "UTF-8-Mobile to Unicode\n";
var_dump(bin2hex(mb_convert_encoding("\xee\x98\xbe", "UCS-4BE", "UTF-8-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xee\xbd\xa0", "UCS-4BE", "UTF-8-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xee\x81\x8a", "UCS-4BE", "UTF-8-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xee\x9d\x81", "UCS-4BE", "UTF-8-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xee\xbf\xac", "UCS-4BE", "UTF-8-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xee\x84\x90", "UCS-4BE", "UTF-8-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xee\x9b\xa0", "UCS-4BE", "UTF-8-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xee\xb6\x89", "UCS-4BE", "UTF-8-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xee\x88\x90", "UCS-4BE", "UTF-8-Mobile#SOFTBANK")));

var_dump(bin2hex(mb_convert_encoding("\xee\xb3\x92", "UCS-4BE", "UTF-8-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xee\x94\x93", "UCS-4BE", "UTF-8-Mobile#SOFTBANK")));

echo "Unicode to UTF8-Mobile\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "UTF-8-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "UTF-8-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "UTF-8-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "UTF-8-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "UTF-8-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf3\x40", "UTF-8-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "UTF-8-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "UTF-8-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x00\x23\x00\x00\x20\xe3", "UTF-8-Mobile#SOFTBANK", "UCS-4BE")));

var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "UTF-8-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "UTF-8-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x01\xf1\xe8\x00\x01\xf1\xf3", "UTF-8-Mobile#SOFTBANK", "UCS-4BE")));

echo "Unicode to ISO-2022-JP-MOBILE#KDDI\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x26\x00", "ISO-2022-JP-MOBILE#KDDI", "UCS-4BE")));

echo "ISO-2022-JP-MOBILE#KDDI to Unicode\n";
var_dump(bin2hex(mb_convert_encoding("\x1b\x24\x42\x75\x41\x1b\x28\x42", "UCS-4BE", "ISO-2022-JP-MOBILE#KDDI")));

echo "SJIS-Mobile to Unicode (Google) \n";
var_dump(bin2hex(mb_convert_encoding("\xf9\x86", "UCS-4BE", "SJIS-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xf7\x48", "UCS-4BE", "SJIS-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xfb\xd8", "UCS-4BE", "SJIS-Mobile#SOFTBANK")));

echo "Unicode (Google) to SJIS-Mobile\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xe8\x2d", "SJIS-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xe8\x2d", "SJIS-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xee\x70", "SJIS-Mobile#SOFTBANK", "UCS-4BE")));

echo "UTF-8-Mobile to Unicode (Google) \n";
var_dump(bin2hex(mb_convert_encoding("\xee\x9b\xa1", "UCS-4BE", "UTF-8-Mobile#DOCOMO")));
var_dump(bin2hex(mb_convert_encoding("\xef\x81\x88", "UCS-4BE", "UTF-8-Mobile#KDDI")));
var_dump(bin2hex(mb_convert_encoding("\xee\x94\xb8", "UCS-4BE", "UTF-8-Mobile#SOFTBANK")));//

echo "Unicode (Google) to UTF-8-Mobile\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xe8\x2d", "UTF-8-Mobile#DOCOMO", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xe8\x2d", "UTF-8-Mobile#KDDI", "UCS-4BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x0f\xee\x70", "UTF-8-Mobile#SOFTBANK", "UCS-4BE")));//

?>
--EXPECT--
SJIS-Mobile to Unicode
string(8) "00002600"
string(8) "00002600"
string(8) "00002600"
string(8) "0001f340"
string(8) "0001f340"
string(8) "0001f340"
string(16) "00000023000020e3"
string(16) "00000023000020e3"
string(16) "00000023000020e3"
string(16) "0001f1e80001f1f3"
string(16) "0001f1e80001f1f3"
Unicode to SJIS-Mobile
string(4) "f89f"
string(4) "f660"
string(4) "f98b"
string(4) "f9e6"
string(4) "f6ec"
string(4) "f750"
string(4) "f985"
string(4) "f489"
string(4) "f7b0"
string(4) "3f3f"
string(4) "f3d2"
string(4) "fbb3"
UTF-8-Mobile to Unicode
string(8) "00002600"
string(8) "00002600"
string(8) "00002600"
string(8) "0001f340"
string(8) "0001f340"
string(8) "0001f340"
string(16) "00000023000020e3"
string(16) "00000023000020e3"
string(16) "00000023000020e3"
string(16) "0001f1e80001f1f3"
string(16) "0001f1e80001f1f3"
Unicode to UTF8-Mobile
string(6) "ee98be"
string(6) "eebda0"
string(6) "ee818a"
string(6) "ee9d81"
string(6) "eebfac"
string(6) "ee8490"
string(6) "ee9ba0"
string(6) "eeb689"
string(6) "ee8890"
string(16) "f09f87a8f09f87b3"
string(6) "eeb392"
string(6) "ee9493"
Unicode to ISO-2022-JP-MOBILE#KDDI
string(16) "1b244275411b2842"
ISO-2022-JP-MOBILE#KDDI to Unicode
string(8) "00002600"
SJIS-Mobile to Unicode (Google) 
string(8) "000fe82d"
string(8) "000fe82d"
string(8) "000fee70"
Unicode (Google) to SJIS-Mobile
string(4) "f986"
string(4) "f748"
string(4) "fbd8"
UTF-8-Mobile to Unicode (Google) 
string(8) "000fe82d"
string(8) "000fe82d"
string(8) "000fee70"
Unicode (Google) to UTF-8-Mobile
string(6) "ee9ba1"
string(6) "ef8188"
string(6) "ee94b8"
