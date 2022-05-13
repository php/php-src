--TEST--
Test mb_decode_numericentity() function : Convert HTML entities to text
--EXTENSIONS--
mbstring
--FILE--
<?php
$str1 = '&#161;&#162;&#163;&#164;&#165;&#166;&#167;&#168;&#169;&#170;&#171;&#172;&#173;&#174;&#175;&#176;&#177;&#178;&#179;&#180;&#181;&#182;&#183;&#184;&#185;&#186;&#187;&#188;&#189;&#190;&#191;&#192;&#193;&#194;&#195;&#196;&#197;&#198;&#199;&#200;&#201;&#202;&#203;&#204;&#205;&#206;&#207;&#208;&#209;&#210;&#211;&#212;&#213;&#214;&#215;&#216;&#217;&#218;&#219;&#220;&#221;&#222;&#223;&#224;&#225;&#226;&#227;&#228;&#229;&#230;&#231;&#232;&#233;&#234;&#235;&#236;&#237;&#238;&#239;&#240;&#241;&#242;&#243;&#244;&#245;&#246;&#247;&#248;&#249;&#250;&#251;&#252;&#253;&#254;&#255;';
$str2 = '&#402;&#913;&#914;&#915;&#916;&#917;&#918;&#919;&#920;&#921;&#922;&#923;&#924;&#925;&#926;&#927;&#928;&#929;&#931;&#932;&#933;&#934;&#935;&#936;&#937;&#945;&#946;&#947;&#948;&#949;&#950;&#951;&#952;&#953;&#954;&#955;&#956;&#957;&#958;&#959;&#960;&#961;&#962;&#963;&#964;&#965;&#966;&#967;&#968;&#969;&#977;&#978;&#982;&#8226;&#8230;&#8242;&#8243;&#8254;&#8260;&#8472;&#8465;&#8476;&#8482;&#8501;&#8592;&#8593;&#8594;&#8595;&#8596;&#8629;&#8656;&#8657;&#8658;&#8659;&#8660;&#8704;&#8706;&#8707;&#8709;&#8711;&#8712;&#8713;&#8715;&#8719;&#8721;&#8722;&#8727;&#8730;&#8733;&#8734;&#8736;&#8743;&#8744;&#8745;&#8746;&#8747;&#8756;&#8764;&#8773;&#8776;&#8800;&#8801;&#8804;&#8805;&#8834;&#8835;&#8836;&#8838;&#8839;&#8853;&#8855;&#8869;&#8901;&#8968;&#8969;&#8970;&#8971;&#9001;&#9002;&#9674;&#9824;&#9827;&#9829;&#9830;';
$str3 = 'a&#338;b&#339;c&#352;d&#353;e&#8364;fg';

$convmap = array(0x0, 0x2FFFF, 0, 0xFFFF);
echo "1: " . mb_decode_numericentity($str1, $convmap, "UTF-8") . "\n";
echo "2: " . mb_decode_numericentity($str2, $convmap, "UTF-8") . "\n";
echo "3: " . mb_decode_numericentity($str3, $convmap, "UTF-8") . "\n";

echo "4: " . mb_decode_numericentity('&#1000000000', $convmap), "\n";
echo "5: " . mb_decode_numericentity('&#9000000000', $convmap), "\n";
echo "6: " . mb_decode_numericentity('&#10000000000', $convmap), "\n";
echo "7: " . mb_decode_numericentity('&#100000000000', $convmap), "\n";

echo "8: " . mb_decode_numericentity('&#000000000000', $convmap), "\n";
echo "9: " . mb_decode_numericentity('&#00000000000', $convmap), "\n";
echo "10: " . mb_decode_numericentity('&#0000000000', $convmap), "\n";
echo "11: " . mb_decode_numericentity('&#000000000', $convmap), "\n";
echo "12: " . bin2hex(mb_decode_numericentity(mb_convert_encoding('&#12345678;', 'UCS-4', 'ASCII'), [0, 0x7FFFFFFF, 0, 0x7FFFFFFF], 'UCS-4')), "\n";

$convmap = [];
echo "13: " . mb_decode_numericentity('f&ouml;o', $convmap, "UTF-8") . "\n";

$convmap = array(0x0, 0x2FFFF, 0); // 3 elements
try {
    echo "14: " . mb_decode_numericentity($str3, $convmap, "UTF-8") . "\n";
} catch (ValueError $ex) {
    echo "14: " . $ex->getMessage()."\n";
}

echo "15: " . bin2hex(mb_decode_numericentity('&#0;', [0, 1, 0, 0xFFFF], 'UTF-8')) . "\n";
echo "16: " . bin2hex(mb_decode_numericentity('&#x0;', [0, 1, 0, 0xFFFF], 'UTF-8')) . "\n";

// Weird convmap
$convmap = [
    0, 0, 0, 0,        // Only one codepoint, empty mask
    100, 50, 0, 0xFFFF // 'End' codepoint is before 'start' codepoint
];
echo "17: " . mb_decode_numericentity('föo', $convmap, "UTF-8") . "\n";

// Convmap with positive offset
$convmap = [0, 10, 1000, 0xFFFF];
echo "18: " . bin2hex(mb_decode_numericentity("&#1001;&#1002;&#1003;", $convmap, "UTF-8")) . "\n";
echo "19: " . bin2hex(mb_decode_numericentity("&#x3E9;&#x3EA;&#x3EB;", $convmap, "UTF-8")) . "\n";

echo "20: " . mb_decode_numericentity("&#123a;", [0, 0xFFFF, 0, 0xFFFF]) . "\n";

?>
--EXPECT--
1: ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ
2: ƒΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρςστυφχψωϑϒϖ•…′″‾⁄℘ℑℜ™ℵ←↑→↓↔↵⇐⇑⇒⇓⇔∀∂∃∅∇∈∉∋∏∑−∗√∝∞∠∧∨∩∪∫∴∼≅≈≠≡≤≥⊂⊃⊄⊆⊇⊕⊗⊥⋅⌈⌉⌊⌋〈〉◊♠♣♥♦
3: aŒbœcŠdše€fg
4: &#1000000000
5: &#9000000000
6: &#10000000000
7: &#100000000000
8: &#000000000000
9: &#00000000000
10: &#0000000000
11: &#000000000
12: 00bc614e
13: f&ouml;o
14: mb_decode_numericentity(): Argument #2 ($map) must have a multiple of 4 elements
15: 00
16: 00
17: föo
18: 010203
19: 010203
20: {a;
