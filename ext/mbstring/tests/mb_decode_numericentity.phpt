--TEST--
Test mb_decode_numericentity() function : Convert HTML entities to text
--EXTENSIONS--
mbstring
--FILE--
<?php

function varDumpToString($var)
{
    ob_start();
    var_dump($var);
    return trim(ob_get_clean());
}

function test($desc, $str, $expected, $convmap, $encoding) {
    $result = mb_decode_numericentity($str, $convmap, $encoding);
    echo $desc, ": ", varDumpToString($str), " => ", varDumpToString($result);
    if ($result === $expected)
        echo " (Good)\n";
    else
        echo " (BAD; expected ", varDumpToString($expected), ")\n";
}

function testNonAscii($desc, $str, $expected, $convmap, $encoding) {
    $result = mb_decode_numericentity($str, $convmap, $encoding);
    echo $desc, ": ", bin2hex($str), " => ", bin2hex($result);
    if ($result === $expected)
        echo " (Good)\n";
    else
        echo " (BAD; expected ", bin2hex($expected), ")\n";
}

$str1 = '&#161;&#162;&#163;&#164;&#165;&#166;&#167;&#168;&#169;&#170;&#171;&#172;&#173;&#174;&#175;&#176;&#177;&#178;&#179;&#180;&#181;&#182;&#183;&#184;&#185;&#186;&#187;&#188;&#189;&#190;&#191;&#192;&#193;&#194;&#195;&#196;&#197;&#198;&#199;&#200;&#201;&#202;&#203;&#204;&#205;&#206;&#207;&#208;&#209;&#210;&#211;&#212;&#213;&#214;&#215;&#216;&#217;&#218;&#219;&#220;&#221;&#222;&#223;&#224;&#225;&#226;&#227;&#228;&#229;&#230;&#231;&#232;&#233;&#234;&#235;&#236;&#237;&#238;&#239;&#240;&#241;&#242;&#243;&#244;&#245;&#246;&#247;&#248;&#249;&#250;&#251;&#252;&#253;&#254;&#255;';
$str2 = '&#402;&#913;&#914;&#915;&#916;&#917;&#918;&#919;&#920;&#921;&#922;&#923;&#924;&#925;&#926;&#927;&#928;&#929;&#931;&#932;&#933;&#934;&#935;&#936;&#937;&#945;&#946;&#947;&#948;&#949;&#950;&#951;&#952;&#953;&#954;&#955;&#956;&#957;&#958;&#959;&#960;&#961;&#962;&#963;&#964;&#965;&#966;&#967;&#968;&#969;&#977;&#978;&#982;&#8226;&#8230;&#8242;&#8243;&#8254;&#8260;&#8472;&#8465;&#8476;&#8482;&#8501;&#8592;&#8593;&#8594;&#8595;&#8596;&#8629;&#8656;&#8657;&#8658;&#8659;&#8660;&#8704;&#8706;&#8707;&#8709;&#8711;&#8712;&#8713;&#8715;&#8719;&#8721;&#8722;&#8727;&#8730;&#8733;&#8734;&#8736;&#8743;&#8744;&#8745;&#8746;&#8747;&#8756;&#8764;&#8773;&#8776;&#8800;&#8801;&#8804;&#8805;&#8834;&#8835;&#8836;&#8838;&#8839;&#8853;&#8855;&#8869;&#8901;&#8968;&#8969;&#8970;&#8971;&#9001;&#9002;&#9674;&#9824;&#9827;&#9829;&#9830;';
$str3 = 'a&#338;b&#339;c&#352;d&#353;e&#8364;fg';

// Typical convmap, typical numeric entity-encoded string
$convmap = array(0x0, 0x2FFFF, 0, 0xFFFF);
echo "1: " . mb_decode_numericentity($str1, $convmap, "UTF-8") . "\n";
echo "2: " . mb_decode_numericentity($str2, $convmap, "UTF-8") . "\n";
echo "3: " . mb_decode_numericentity($str3, $convmap, "UTF-8") . "\n";

// Numeric entities which are truncated at end of string
echo "4: " . mb_decode_numericentity('&#1000000000', $convmap), "\n"; // Entity is too big
echo "5: " . mb_decode_numericentity('&#9000000000', $convmap), "\n"; // Entity is too big
echo "6: " . mb_decode_numericentity('&#10000000000', $convmap), "\n"; // Too many digits
echo "7: " . mb_decode_numericentity('&#100000000000', $convmap), "\n"; // Too many digits
echo "8: " . mb_decode_numericentity('&#000000000000', $convmap), "\n"; // Too many digits
echo "9: " . mb_decode_numericentity('&#00000000000', $convmap), "\n"; // Too many digits
echo "10: " . bin2hex(mb_decode_numericentity('&#0000000000', $convmap)), "\n"; // OK
echo "11: " . bin2hex(mb_decode_numericentity('&#000000000', $convmap)), "\n"; // OK
// Try with hex, not just decimal entities
echo "11b: " . bin2hex(mb_decode_numericentity('&#x0000000', $convmap)), "\n"; // OK
echo "11c: " . bin2hex(mb_decode_numericentity('&#x00000000', $convmap)), "\n"; // OK
echo "11d: " . bin2hex(mb_decode_numericentity('&#x10000', $convmap)), "\n"; // OK
echo "11e: " . mb_decode_numericentity('&#x000000000', $convmap), "\n"; // Too many digits

// Large decimal entity, converting from non-ASCII input encoding
echo "12: " . bin2hex(mb_decode_numericentity(mb_convert_encoding('&#12345678;', 'UCS-4', 'ASCII'), [0, 0x7FFFFFFF, 0, 0x7FFFFFFF], 'UCS-4')), "\n";

$convmap = [];
echo "13: " . mb_decode_numericentity('f&ouml;o', $convmap, "UTF-8") . "\n";

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

test("10 digits for decimal entity", "&#0000000065;", "A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("More than 10 digits for decimal entity", "&#00000000165;", "&#00000000165;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("8 digits for hex entity", "&#x00000041;", "A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("More than 8 digits for hex entity", "&#x000000141;", "&#x000000141;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("Single &", "&", "&", [0, 0xFFFF, 0, 0xFFFF], "ASCII");

// An entity can come right after a preceding ampersand
test("Successive &", "&&#65,", "&A,", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// An entity can come right after a preceding &#
test("Successive &#", "&#&#x32;", "&#2", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Successive &#x", "&#x&#x32;", "&#x2", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("&#x only", "&#x;", "&#x;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// The starting & of an entity can terminate a preceding entity
test("Successive &#65", "&#65&#65;", "AA", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Successive hex entities", "&#x32&#x32;", "22", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// An entity can come right after an entity which is invalid because of being too long
test("Starting entity immediately after decimal entity which is too long", "&#10000000000&#65;", "&#10000000000A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Starting entity immediately after hex entity which is too long", "&#x111111111&#65;", "&#x111111111A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("Starting entity immediately after invalid decimal entity", "&#0&#65;", "&#0A", [0x1, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Starting entity immediately after invalid hex entity", "&#x0&#65;", "&#x0A", [0x1, 0xFFFF, 0, 0xFFFF], 'ASCII');

// Try with '&', '&#', or '&#' at the end of a buffer of wchars, with more input
// still left to process in the next buffer
// (mb_decode_numericentity splits its input into 'chunks' and processes it one
// chunk at a time)
$convmap = [0, 0xFFFF, 0, 0xFFFF];
for ($i = 0; $i < 256; $i++) {
    $padding = str_repeat("a", $i);
    // First try invalid decimal/hex entities
    if (mb_decode_numericentity($padding . "&#ZZZ", $convmap, 'UTF-8') !== $padding . "&#ZZZ")
        die("&#ZZZ is broken when it spans two buffers!");
    if (mb_decode_numericentity($padding . "&#xZZZ", $convmap, 'UTF-8') !== $padding . "&#xZZZ")
        die("&#xZZZ is broken when it spans two buffers!");
    // Now try valid decimal/hex entities
    if (mb_decode_numericentity($padding . "&#65", $convmap, 'UTF-8') !== $padding . "A")
        die("&#65 is broken when it spans two buffers!");
    if (mb_decode_numericentity($padding . "&#x41", $convmap, 'UTF-8') !== $padding . "A")
        die("&#x41 is broken when it spans two buffers!");
}

// Try huge entities, big enough to fill an entire buffer
for ($i = 12; $i < 256; $i++) {
    $str = "&#" . str_repeat("0", $i) . "65";
    if (mb_decode_numericentity($str, $convmap, 'UTF-8') !== $str)
        die("Decimal entity with huge number of digits broken");

    $str = "&#x" . str_repeat("0", $i) . "41";
    if (mb_decode_numericentity($str, $convmap, 'UTF-8') !== $str)
        die("Hexadecimal entity with huge number of digits broken");
}

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
10: 00
11: 00
11b: 00
11c: 00
11d: f0908080
11e: &#x000000000
12: 00bc614e
13: f&ouml;o
15: 00
16: 00
17: föo
18: 010203
19: 010203
20: {a;
10 digits for decimal entity: string(13) "&#0000000065;" => string(1) "A" (Good)
More than 10 digits for decimal entity: string(14) "&#00000000165;" => string(14) "&#00000000165;" (Good)
8 digits for hex entity: string(12) "&#x00000041;" => string(1) "A" (Good)
More than 8 digits for hex entity: string(13) "&#x000000141;" => string(13) "&#x000000141;" (Good)
Single &: string(1) "&" => string(1) "&" (Good)
Successive &: string(6) "&&#65," => string(3) "&A," (Good)
Successive &#: string(8) "&#&#x32;" => string(3) "&#2" (Good)
Successive &#x: string(9) "&#x&#x32;" => string(4) "&#x2" (Good)
&#x only: string(4) "&#x;" => string(4) "&#x;" (Good)
Successive &#65: string(9) "&#65&#65;" => string(2) "AA" (Good)
Successive hex entities: string(11) "&#x32&#x32;" => string(2) "22" (Good)
Starting entity immediately after decimal entity which is too long: string(18) "&#10000000000&#65;" => string(14) "&#10000000000A" (Good)
Starting entity immediately after hex entity which is too long: string(17) "&#x111111111&#65;" => string(13) "&#x111111111A" (Good)
Starting entity immediately after invalid decimal entity: string(8) "&#0&#65;" => string(4) "&#0A" (Good)
Starting entity immediately after invalid hex entity: string(9) "&#x0&#65;" => string(5) "&#x0A" (Good)
