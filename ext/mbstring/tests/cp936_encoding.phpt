--TEST--
Exhaustive test of verification and conversion of CP936 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');

srand(1000); // Make results consistent
mb_substitute_character(0x25); // '%'
readConversionTable(__DIR__ . '/data/CP936.txt', $toUnicode, $fromUnicode);

// Unicode has more than one codepoint for a 'tilde' character
// On output, we emit U+FF5E FULLWIDTH TILDE, but we accept U+223C as input
$fromUnicode["\x22\x3c"] = "\xa1\xab";

// Circle
// On output, we emit U+00B0 RING OPERATOR, but we accept U+2218 as input
$fromUnicode["\x22\x18"] = "\xa1\xe3";

// Overline
// On output, we emit U+FFE3 FULLWIDTH MACRON, but we accept U+203E as input
$fromUnicode["\x20\x3e"] = "\xa3\xfe";

// We support some CJK compatibility ideographs
// Ref: https://en.wikipedia.org/wiki/CJK_Compatibility_Ideographs
// These are accepted when converting Unicode -> CP936, but are not produced
// when converting CP936 -> Unicode
$fromUnicode["\xf9\x00"] = "\xd8\x4d";
$fromUnicode["\xf9\x01"] = "\xb8\xfc";
$fromUnicode["\xf9\x02"] = "\xdc\x87";
$fromUnicode["\xf9\x03"] = "\xd9\x5a";
$fromUnicode["\xf9\x04"] = "\xbb\xac";
$fromUnicode["\xf9\x05"] = "\xb4\xae";
$fromUnicode["\xf9\x06"] = "\xbe\xe4";
$fromUnicode["\xf9\x07"] = "\xfd\x94";
$fromUnicode["\xf9\x08"] = "\xfd\x94";
$fromUnicode["\xf9\x09"] = "\xc6\xf5";
$fromUnicode["\xf9\x0a"] = "\xbd\xf0";
$fromUnicode["\xf9\x0b"] = "\xc0\xae";
$fromUnicode["\xf9\x0c"] = "\xc4\xce";
$fromUnicode["\xf9\x0d"] = "\x91\xd0";
$fromUnicode["\xf9\x0e"] = "\xb0\x5d";
$fromUnicode["\xf9\x0f"] = "\xc1\x5f";
$fromUnicode["\xf9\x10"] = "\xcc\x7d";
$fromUnicode["\xf9\x11"] = "\xc2\xdd";
$fromUnicode["\xf9\x12"] = "\xc2\xe3";
$fromUnicode["\xf9\x13"] = "\xdf\x89";
$fromUnicode["\xf9\x14"] = "\x98\xb7";
$fromUnicode["\xf9\x15"] = "\xc2\xe5";
$fromUnicode["\xf9\x16"] = "\xc0\xd3";
$fromUnicode["\xf9\x17"] = "\xe7\xf3";
$fromUnicode["\xf9\x18"] = "\xc2\xe4";
$fromUnicode["\xf9\x19"] = "\xc0\xd2";
$fromUnicode["\xf9\x1a"] = "\xf1\x98";
$fromUnicode["\xf9\x1b"] = "\x81\x79";
$fromUnicode["\xf9\x1c"] = "\xc2\xd1";
$fromUnicode["\xf9\x1d"] = "\x99\xda";
$fromUnicode["\xf9\x1e"] = "\xa0\x80";
$fromUnicode["\xf9\x1f"] = "\xcc\x6d";
$fromUnicode["\xf9\x20"] = "\xfb\x5b";
$fromUnicode["\xf9\x21"] = "\x8d\xb9";
$fromUnicode["\xf9\x22"] = "\x9e\x45";
$fromUnicode["\xf9\x23"] = "\xcb\x7b";
$fromUnicode["\xf9\x24"] = "\xd2\x68";
$fromUnicode["\xf9\x25"] = "\xc0\xad";
$fromUnicode["\xf9\x26"] = "\xc5\x44";
$fromUnicode["\xf9\x27"] = "\xcf\x9e";
$fromUnicode["\xf9\x28"] = "\xc0\xc8";
$fromUnicode["\xf9\x29"] = "\xc0\xca";
$fromUnicode["\xf9\x2a"] = "\xc0\xcb";
$fromUnicode["\xf9\x2b"] = "\xc0\xc7";
$fromUnicode["\xf9\x2c"] = "\xfd\x9c";
$fromUnicode["\xf9\x2d"] = "\x81\xed";
$fromUnicode["\xf9\x2e"] = "\xc0\xe4";
$fromUnicode["\xf9\x2f"] = "\x84\xda";
$fromUnicode["\xf9\x30"] = "\x93\xef";
$fromUnicode["\xf9\x31"] = "\x99\xa9";
$fromUnicode["\xf9\x32"] = "\xa0\x74";
$fromUnicode["\xf9\x33"] = "\xb1\x52";
$fromUnicode["\xf9\x34"] = "\xc0\xcf";
$fromUnicode["\xf9\x35"] = "\xcc\x4a";
$fromUnicode["\xf9\x36"] = "\xcc\x94";
$fromUnicode["\xf9\x37"] = "\xc2\xb7";
$fromUnicode["\xf9\x38"] = "\xc2\xb6";
$fromUnicode["\xf9\x39"] = "\xf4\x94";
$fromUnicode["\xf9\x3a"] = "\xfa\x98";
$fromUnicode["\xf9\x3b"] = "\xc2\xb5";
$fromUnicode["\xf9\x3c"] = "\xb5\x93";
$fromUnicode["\xf9\x3d"] = "\xbe\x47";
$fromUnicode["\xf9\x3e"] = "\xc7\x8a";
$fromUnicode["\xf9\x3f"] = "\xe4\x9b";
$fromUnicode["\xf9\x40"] = "\xc2\xb9";
$fromUnicode["\xf9\x41"] = "\xd5\x93";
$fromUnicode["\xf9\x42"] = "\x89\xc5";
$fromUnicode["\xf9\x43"] = "\xc5\xaa";
$fromUnicode["\xf9\x44"] = "\xbb\x5c";
$fromUnicode["\xf9\x45"] = "\xc3\x40";
$fromUnicode["\xf9\x46"] = "\xc0\xce";
$fromUnicode["\xf9\x47"] = "\xc0\xda";
$fromUnicode["\xf9\x48"] = "\xd9\x54";
$fromUnicode["\xf9\x49"] = "\xc0\xd7";
$fromUnicode["\xf9\x4a"] = "\x89\xbe";
$fromUnicode["\xf9\x4b"] = "\x8c\xd2";
$fromUnicode["\xf9\x4c"] = "\x98\xc7";
$fromUnicode["\xf9\x4d"] = "\x9c\x49";
$fromUnicode["\xf9\x4e"] = "\xc2\xa9";
$fromUnicode["\xf9\x4f"] = "\xc0\xdb";
$fromUnicode["\xf9\x50"] = "\xbf\x7c";
$fromUnicode["\xf9\x51"] = "\xc2\xaa";
$fromUnicode["\xf9\x52"] = "\xc0\xd5";
$fromUnicode["\xf9\x53"] = "\xc0\xdf";
$fromUnicode["\xf9\x54"] = "\x84\x43";
$fromUnicode["\xf9\x55"] = "\xc1\xe8";
$fromUnicode["\xf9\x56"] = "\xb6\xa0";
$fromUnicode["\xf9\x57"] = "\xbe\x63";
$fromUnicode["\xf9\x58"] = "\xc1\xe2";
$fromUnicode["\xf9\x59"] = "\xc1\xea";
$fromUnicode["\xf9\x5a"] = "\xd7\x78";
$fromUnicode["\xf9\x5b"] = "\x92\x82";
$fromUnicode["\xf9\x5c"] = "\x98\xb7";
$fromUnicode["\xf9\x5d"] = "\xd6\x5a";
$fromUnicode["\xf9\x5e"] = "\xb5\xa4";
$fromUnicode["\xf9\x5f"] = "\x8c\x8e";
$fromUnicode["\xf9\x60"] = "\xc5\xad";
$fromUnicode["\xf9\x61"] = "\xc2\xca";
$fromUnicode["\xf9\x62"] = "\xae\x90";
$fromUnicode["\xf9\x63"] = "\xb1\xb1";
$fromUnicode["\xf9\x64"] = "\xb4\x91";
$fromUnicode["\xf9\x65"] = "\xb1\xe3";
$fromUnicode["\xf9\x66"] = "\x8f\xcd";
$fromUnicode["\xf9\x67"] = "\xb2\xbb";
$fromUnicode["\xf9\x68"] = "\xc3\xda";
$fromUnicode["\xf9\x69"] = "\x94\xb5";
$fromUnicode["\xf9\x6a"] = "\xcb\xf7";
$fromUnicode["\xf9\x6b"] = "\x85\xa2";
$fromUnicode["\xf9\x6c"] = "\xc8\xfb";
$fromUnicode["\xf9\x6d"] = "\xca\xa1";
$fromUnicode["\xf9\x6e"] = "\xc8\x7e";
$fromUnicode["\xf9\x6f"] = "\xd5\x66";
$fromUnicode["\xf9\x70"] = "\x9a\xa2";
$fromUnicode["\xf9\x71"] = "\xb3\xbd";
$fromUnicode["\xf9\x72"] = "\xc9\xf2";
$fromUnicode["\xf9\x73"] = "\xca\xb0";
$fromUnicode["\xf9\x74"] = "\xc8\xf4";
$fromUnicode["\xf9\x75"] = "\xc2\xd3";
$fromUnicode["\xf9\x76"] = "\xc2\xd4";
$fromUnicode["\xf9\x77"] = "\xc1\xc1";
$fromUnicode["\xf9\x78"] = "\x83\xc9";
$fromUnicode["\xf9\x7a"] = "\xc1\xba";
$fromUnicode["\xf9\x7b"] = "\xbc\x5a";
$fromUnicode["\xf9\x7c"] = "\xc1\xbc";
$fromUnicode["\xf9\x7d"] = "\xd5\x8f";
$fromUnicode["\xf9\x7e"] = "\xc1\xbf";
$fromUnicode["\xf9\x7f"] = "\x84\xee";
$fromUnicode["\xf9\x80"] = "\x85\xce";
$fromUnicode["\xf9\x81"] = "\xc5\xae";
$fromUnicode["\xf9\x82"] = "\x8f\x5d";
$fromUnicode["\xf9\x83"] = "\xc2\xc3";
$fromUnicode["\xf9\x84"] = "\x9e\x56";
$fromUnicode["\xf9\x85"] = "\xb5\x5a";
$fromUnicode["\xf9\x86"] = "\xe9\x82";
$fromUnicode["\xf9\x87"] = "\xf3\x50";
$fromUnicode["\xf9\x88"] = "\xfb\x90";
$fromUnicode["\xf9\x89"] = "\xc0\xe8";
$fromUnicode["\xf9\x8a"] = "\xc1\xa6";
$fromUnicode["\xf9\x8b"] = "\x95\xd1";
$fromUnicode["\xf9\x8c"] = "\x9a\x76";
$fromUnicode["\xf9\x8d"] = "\xde\x5d";
$fromUnicode["\xf9\x8e"] = "\xc4\xea";
$fromUnicode["\xf9\x8f"] = "\x91\x7a";
$fromUnicode["\xf9\x90"] = "\x91\xd9";
$fromUnicode["\xf9\x91"] = "\x93\xd3";
$fromUnicode["\xf9\x92"] = "\x9d\x69";
$fromUnicode["\xf9\x93"] = "\x9f\x92";
$fromUnicode["\xf9\x94"] = "\xad\x49";
$fromUnicode["\xf9\x95"] = "\xfd\x9e";
$fromUnicode["\xf9\x96"] = "\xbe\x9a";
$fromUnicode["\xf9\x97"] = "\xc2\x93";
$fromUnicode["\xf9\x98"] = "\xdd\x82";
$fromUnicode["\xf9\x99"] = "\xc9\x8f";
$fromUnicode["\xf9\x9a"] = "\xdf\x42";
$fromUnicode["\xf9\x9b"] = "\xe5\x80";
$fromUnicode["\xf9\x9c"] = "\xc1\xd0";
$fromUnicode["\xf9\x9d"] = "\xc1\xd3";
$fromUnicode["\xf9\x9e"] = "\xd1\xca";
$fromUnicode["\xf9\x9f"] = "\xc1\xd2";
$fromUnicode["\xf9\xa0"] = "\xc1\xd1";
$fromUnicode["\xf9\xa1"] = "\xd5\x66";
$fromUnicode["\xf9\xa2"] = "\xc1\xae";
$fromUnicode["\xf9\xa3"] = "\xc4\xee";
$fromUnicode["\xf9\xa4"] = "\xc4\xed";
$fromUnicode["\xf9\xa5"] = "\x9a\x9a";
$fromUnicode["\xf9\xa6"] = "\xba\x9f";
$fromUnicode["\xf9\xa7"] = "\xab\x43";
$fromUnicode["\xf9\xa8"] = "\xc1\xee";
$fromUnicode["\xf9\xa9"] = "\xe0\xf2";
$fromUnicode["\xf9\xaa"] = "\x8c\x8e";
$fromUnicode["\xf9\xab"] = "\x8e\x58";
$fromUnicode["\xf9\xac"] = "\xc1\xaf";
$fromUnicode["\xf9\xad"] = "\xc1\xe1";
$fromUnicode["\xf9\xae"] = "\xac\x93";
$fromUnicode["\xf9\xaf"] = "\xc1\xe7";
$fromUnicode["\xf9\xb0"] = "\xf1\xf6";
$fromUnicode["\xf9\xb1"] = "\xe2\x8f";
$fromUnicode["\xf9\xb2"] = "\xc1\xe3";
$fromUnicode["\xf9\xb3"] = "\xec\x60";
$fromUnicode["\xf9\xb4"] = "\xee\x49";
$fromUnicode["\xf9\xb5"] = "\xc0\xfd";
$fromUnicode["\xf9\xb6"] = "\xb6\x59";
$fromUnicode["\xf9\xb7"] = "\xf5\xb7";
$fromUnicode["\xf9\xb8"] = "\xeb\x60";
$fromUnicode["\xf9\xb9"] = "\x90\xba";
$fromUnicode["\xf9\xba"] = "\xc1\xcb";
$fromUnicode["\xf9\xbb"] = "\xc1\xc5";
$fromUnicode["\xf9\xbc"] = "\xe5\xbc";
$fromUnicode["\xf9\xbd"] = "\xc4\xf2";
$fromUnicode["\xf9\xbe"] = "\xc1\xcf";
$fromUnicode["\xf9\xbf"] = "\x98\xb7";
$fromUnicode["\xf9\xc0"] = "\xc1\xc7";
$fromUnicode["\xf9\xc1"] = "\xaf\x9f";
$fromUnicode["\xf9\xc2"] = "\xde\xa4";
$fromUnicode["\xf9\xc3"] = "\xdf\x7c";
$fromUnicode["\xf9\xc4"] = "\xfd\x88";
$fromUnicode["\xf9\xc5"] = "\x95\x9e";
$fromUnicode["\xf9\xc6"] = "\xc8\xee";
$fromUnicode["\xf9\xc7"] = "\x84\xa2";
$fromUnicode["\xf9\xc8"] = "\x96\x83";
$fromUnicode["\xf9\xc9"] = "\xc1\xf8";
$fromUnicode["\xf9\xca"] = "\xc1\xf7";
$fromUnicode["\xf9\xcb"] = "\xc1\xef";
$fromUnicode["\xf9\xcc"] = "\xc1\xf0";
$fromUnicode["\xf9\xcd"] = "\xc1\xf4";
$fromUnicode["\xf9\xce"] = "\xc1\xf2";
$fromUnicode["\xf9\xcf"] = "\xbc\x7e";
$fromUnicode["\xf9\xd0"] = "\xee\x90";
$fromUnicode["\xf9\xd1"] = "\xc1\xf9";
$fromUnicode["\xf9\xd2"] = "\xc2\xbe";
$fromUnicode["\xf9\xd3"] = "\xea\x91";
$fromUnicode["\xf9\xd4"] = "\x82\x90";
$fromUnicode["\xf9\xd5"] = "\x8d\x91";
$fromUnicode["\xf9\xd6"] = "\x9c\x53";
$fromUnicode["\xf9\xd7"] = "\xdd\x86";
$fromUnicode["\xf9\xd8"] = "\xc2\xc9";
$fromUnicode["\xf9\xd9"] = "\x90\xfc";
$fromUnicode["\xf9\xda"] = "\xc0\xf5";
$fromUnicode["\xf9\xdb"] = "\xc2\xca";
$fromUnicode["\xf9\xdc"] = "\xc2\xa1";
$fromUnicode["\xf9\xdd"] = "\xc0\xfb";
$fromUnicode["\xf9\xde"] = "\xc0\xf4";
$fromUnicode["\xf9\xdf"] = "\xc2\xc4";
$fromUnicode["\xf9\xe0"] = "\xd2\xd7";
$fromUnicode["\xf9\xe1"] = "\xc0\xee";
$fromUnicode["\xf9\xe2"] = "\xc0\xe6";
$fromUnicode["\xf9\xe3"] = "\xc4\xe0";
$fromUnicode["\xf9\xe4"] = "\xc0\xed";
$fromUnicode["\xf9\xe5"] = "\xc1\xa1";
$fromUnicode["\xf9\xe6"] = "\xee\xbe";
$fromUnicode["\xf9\xe8"] = "\xd1\x65";
$fromUnicode["\xf9\xe9"] = "\xc0\xef";
$fromUnicode["\xf9\xea"] = "\xeb\x78";
$fromUnicode["\xf9\xeb"] = "\xc4\xe4";
$fromUnicode["\xf9\xec"] = "\xc4\xe7";
$fromUnicode["\xf9\xed"] = "\xc1\xdf";
$fromUnicode["\xf9\xee"] = "\x9f\xfb";
$fromUnicode["\xf9\xef"] = "\xad\x55";
$fromUnicode["\xf9\xf0"] = "\xcc\x41";
$fromUnicode["\xf9\xf2"] = "\xf7\x5b";
$fromUnicode["\xf9\xf3"] = "\xf7\xeb";
$fromUnicode["\xf9\xf4"] = "\xc1\xd6";
$fromUnicode["\xf9\xf5"] = "\xc1\xdc";
$fromUnicode["\xf9\xf6"] = "\xc5\x52";
$fromUnicode["\xf9\xf7"] = "\xc1\xa2";
$fromUnicode["\xf9\xf8"] = "\xf3\xd2";
$fromUnicode["\xf9\xf9"] = "\xc1\xa3";
$fromUnicode["\xf9\xfa"] = "\xa0\xee";
$fromUnicode["\xf9\xfb"] = "\xd6\xcb";
$fromUnicode["\xf9\xfc"] = "\xd7\x52";
$fromUnicode["\xf9\xfd"] = "\xca\xb2";
$fromUnicode["\xf9\xfe"] = "\xb2\xe8";
$fromUnicode["\xf9\xff"] = "\xb4\xcc";
$fromUnicode["\xfa\x00"] = "\xc7\xd0";
$fromUnicode["\xfa\x01"] = "\xb6\xc8";
$fromUnicode["\xfa\x02"] = "\xcd\xd8";
$fromUnicode["\xfa\x03"] = "\xcc\xc7";
$fromUnicode["\xfa\x04"] = "\xd5\xac";
$fromUnicode["\xfa\x05"] = "\xb6\xb4";
$fromUnicode["\xfa\x06"] = "\xb1\xa9";
$fromUnicode["\xfa\x07"] = "\xdd\x97";
$fromUnicode["\xfa\x08"] = "\xd0\xd0";
$fromUnicode["\xfa\x09"] = "\xbd\xb5";
$fromUnicode["\xfa\x0a"] = "\xd2\x8a";
$fromUnicode["\xfa\x0b"] = "\xc0\xaa";
$fromUnicode["\xfa\x10"] = "\x89\x56";
$fromUnicode["\xfa\x12"] = "\xc7\xe7";
$fromUnicode["\xfa\x15"] = "\x84\x44";
$fromUnicode["\xfa\x16"] = "\xd8\x69";
$fromUnicode["\xfa\x17"] = "\xd2\xe6";
$fromUnicode["\xfa\x19"] = "\xc9\xf1";
$fromUnicode["\xfa\x1a"] = "\xcf\xe9";
$fromUnicode["\xfa\x1b"] = "\xb8\xa3";
$fromUnicode["\xfa\x1c"] = "\xbe\xb8";
$fromUnicode["\xfa\x1d"] = "\xbe\xab";
$fromUnicode["\xfa\x1e"] = "\xd3\xf0";
$fromUnicode["\xfa\x22"] = "\xd6\x54";
$fromUnicode["\xfa\x25"] = "\xd2\xdd";
$fromUnicode["\xfa\x26"] = "\xb6\xbc";
$fromUnicode["\xfa\x2a"] = "\xef\x88";
$fromUnicode["\xfa\x2b"] = "\xef\x95";
$fromUnicode["\xfa\x2c"] = "\xf0\x5e";
$fromUnicode["\xfa\x2d"] = "\xfa\x51";

findInvalidChars($toUnicode, $invalid, $truncated);
testAllValidChars($toUnicode, 'CP936', 'UTF-16BE');
testAllInvalidChars($invalid, $toUnicode, 'CP936', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'CP936', 'UTF-16BE', "\x00%");
echo "Tested CP936 -> UTF-16BE\n";

findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'CP936', '%');
echo "Tested UTF-16BE -> CP936\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x81\x20", "%", "CP936", "UTF-8");
convertInvalidString("\x81\x7F", "%", "CP936", "UTF-8");
convertInvalidString("\xFE\xFF", "%", "CP936", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested CP936 -> UTF-16BE
Tested UTF-16BE -> CP936
Done!
