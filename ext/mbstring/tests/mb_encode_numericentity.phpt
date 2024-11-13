--TEST--
Test mb_encode_numericentity() function: Convert text to HTML entities
--EXTENSIONS--
mbstring
--FILE--
<?php
$str1 = '¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ';
$str2 = 'ƒΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρςστυφχψωϑϒϖ•…′″‾⁄℘ℑℜ™ℵ←↑→↓↔↵⇐⇑⇒⇓⇔∀∂∃∅∇∈∉∋∏∑−∗√∝∞∠∧∨∩∪∫∴∼≅≈≠≡≤≥⊂⊃⊄⊆⊇⊕⊗⊥⋅⌈⌉⌊⌋〈〉◊♠♣♥♦';
$convmap = array(0x0, 0x2FFFF, 0, 0xFFFF);
echo "1: " . mb_encode_numericentity($str1, $convmap, "UTF-8") . "\n";
echo "1 (hex): " . mb_encode_numericentity($str1, $convmap, "UTF-8", true) . "\n";
echo "2: " . mb_encode_numericentity($str2, $convmap, "UTF-8") . "\n";

$convmap = array(0xFF, 0x2FFFF, 0, 0xFFFF);
echo "3: " . mb_encode_numericentity('aŒbœcŠdše€fg', $convmap, "UTF-8") . "\n";

$convmap = [];
echo "4: " . mb_encode_numericentity('föo', $convmap, "UTF-8") . "\n";

// HTML-encode a null byte
echo "6: " . mb_encode_numericentity("\x00", array(0, 1, 0, 0xFFFF), "UTF-8", false) . "\n";
echo "6 (hex): " . mb_encode_numericentity("\x00", array(0, 1, 0, 0xFFFF), "UTF-8", true) . "\n";

// Try doing weird things with convmap
$convmap = [
    0, 0, 0, 0,        // Only one codepoint, empty mask
    100, 50, 0, 0xFFFF // 'End' codepoint is before 'start' codepoint
];
echo "7: " . mb_encode_numericentity('föo', $convmap, "UTF-8") . "\n";
echo "7 (hex): " . mb_encode_numericentity('föo', $convmap, "UTF-8", true) . "\n";

// Try using positive offset
$convmap = [0, 10, 1000, 0xFFFF];
echo "8: " . mb_encode_numericentity("\x01\x02\x03", $convmap, "UTF-8") . "\n";
echo "8 (hex): " . mb_encode_numericentity("\x01\x02\x03", $convmap, "UTF-8", true) . "\n";

// Try using negative offset
$convmap = [0, 10, -100000, 0xFFFFFF];
echo "9: " . mb_encode_numericentity("\x01\x02\x03", $convmap, "UTF-8") . "\n";
echo "9 (hex): " . mb_encode_numericentity("\x01\x02\x03", $convmap, "UTF-8", true) . "\n";

// Try using mask to remove some bits
$convmap = [0, 1000, 0, 0x123];
echo "10: " . mb_encode_numericentity('föo', $convmap, "UTF-8") . "\n";
echo "10 (hex): " . mb_encode_numericentity('föo', $convmap, "UTF-8", true) . "\n";

// Try different text encoding
$convmap = [0, 0xFFFF, 0, 0xFFFF];
$iso2022jp = "\x1B\$B\x21\x21!r\x1B(BABC";
echo "11: " . mb_encode_numericentity($iso2022jp, $convmap, "ISO-2022-JP") . "\n";
echo "11 (hex): " . mb_encode_numericentity($iso2022jp, $convmap, "ISO-2022-JP", true) . "\n";

// Regression test; the old implementation could only emit decimal entities with about 7 digits
$convmap = [0x2b, 0x2d4, 0x75656500, 0x656d2c53];
echo "12: " . mb_encode_numericentity("m", $convmap, "ASCII") . "\n";

?>
--EXPECT--
1: &#161;&#162;&#163;&#164;&#165;&#166;&#167;&#168;&#169;&#170;&#171;&#172;&#173;&#174;&#175;&#176;&#177;&#178;&#179;&#180;&#181;&#182;&#183;&#184;&#185;&#186;&#187;&#188;&#189;&#190;&#191;&#192;&#193;&#194;&#195;&#196;&#197;&#198;&#199;&#200;&#201;&#202;&#203;&#204;&#205;&#206;&#207;&#208;&#209;&#210;&#211;&#212;&#213;&#214;&#215;&#216;&#217;&#218;&#219;&#220;&#221;&#222;&#223;&#224;&#225;&#226;&#227;&#228;&#229;&#230;&#231;&#232;&#233;&#234;&#235;&#236;&#237;&#238;&#239;&#240;&#241;&#242;&#243;&#244;&#245;&#246;&#247;&#248;&#249;&#250;&#251;&#252;&#253;&#254;&#255;
1 (hex): &#xA1;&#xA2;&#xA3;&#xA4;&#xA5;&#xA6;&#xA7;&#xA8;&#xA9;&#xAA;&#xAB;&#xAC;&#xAD;&#xAE;&#xAF;&#xB0;&#xB1;&#xB2;&#xB3;&#xB4;&#xB5;&#xB6;&#xB7;&#xB8;&#xB9;&#xBA;&#xBB;&#xBC;&#xBD;&#xBE;&#xBF;&#xC0;&#xC1;&#xC2;&#xC3;&#xC4;&#xC5;&#xC6;&#xC7;&#xC8;&#xC9;&#xCA;&#xCB;&#xCC;&#xCD;&#xCE;&#xCF;&#xD0;&#xD1;&#xD2;&#xD3;&#xD4;&#xD5;&#xD6;&#xD7;&#xD8;&#xD9;&#xDA;&#xDB;&#xDC;&#xDD;&#xDE;&#xDF;&#xE0;&#xE1;&#xE2;&#xE3;&#xE4;&#xE5;&#xE6;&#xE7;&#xE8;&#xE9;&#xEA;&#xEB;&#xEC;&#xED;&#xEE;&#xEF;&#xF0;&#xF1;&#xF2;&#xF3;&#xF4;&#xF5;&#xF6;&#xF7;&#xF8;&#xF9;&#xFA;&#xFB;&#xFC;&#xFD;&#xFE;&#xFF;
2: &#402;&#913;&#914;&#915;&#916;&#917;&#918;&#919;&#920;&#921;&#922;&#923;&#924;&#925;&#926;&#927;&#928;&#929;&#931;&#932;&#933;&#934;&#935;&#936;&#937;&#945;&#946;&#947;&#948;&#949;&#950;&#951;&#952;&#953;&#954;&#955;&#956;&#957;&#958;&#959;&#960;&#961;&#962;&#963;&#964;&#965;&#966;&#967;&#968;&#969;&#977;&#978;&#982;&#8226;&#8230;&#8242;&#8243;&#8254;&#8260;&#8472;&#8465;&#8476;&#8482;&#8501;&#8592;&#8593;&#8594;&#8595;&#8596;&#8629;&#8656;&#8657;&#8658;&#8659;&#8660;&#8704;&#8706;&#8707;&#8709;&#8711;&#8712;&#8713;&#8715;&#8719;&#8721;&#8722;&#8727;&#8730;&#8733;&#8734;&#8736;&#8743;&#8744;&#8745;&#8746;&#8747;&#8756;&#8764;&#8773;&#8776;&#8800;&#8801;&#8804;&#8805;&#8834;&#8835;&#8836;&#8838;&#8839;&#8853;&#8855;&#8869;&#8901;&#8968;&#8969;&#8970;&#8971;&#9001;&#9002;&#9674;&#9824;&#9827;&#9829;&#9830;
3: a&#338;b&#339;c&#352;d&#353;e&#8364;fg
4: föo
6: &#0;
6 (hex): &#x0;
7: föo
7 (hex): föo
8: &#1001;&#1002;&#1003;
8 (hex): &#x3E9;&#x3EA;&#x3EB;
9: &#16677217;&#16677218;&#16677219;
9 (hex): &#xFE7961;&#xFE7962;&#xFE7963;
10: &#34;&#34;&#35;
10 (hex): &#x22;&#x22;&#x23;
11: &#12288;&#163;&#65;&#66;&#67;
11 (hex): &#x3000;&#xA3;&#x41;&#x42;&#x43;
12: &#1701127233;
