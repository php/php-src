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
// We do NOT decode such entities; they can be terminated by any non-digit character, but not by the end of the string
echo "4: " . mb_decode_numericentity('&#1000000000', $convmap), "\n";
echo "5: " . mb_decode_numericentity('&#9000000000', $convmap), "\n";
echo "6: " . mb_decode_numericentity('&#10000000000', $convmap), "\n";
echo "7: " . mb_decode_numericentity('&#100000000000', $convmap), "\n";
echo "8: " . mb_decode_numericentity('&#000000000000', $convmap), "\n";
echo "9: " . mb_decode_numericentity('&#00000000000', $convmap), "\n";
echo "10: " . mb_decode_numericentity('&#0000000000', $convmap), "\n";
echo "11: " . mb_decode_numericentity('&#000000000', $convmap), "\n";
// Try with hex, not just decimal entities
echo "11b: " . mb_decode_numericentity('&#x0000000', $convmap), "\n";
echo "11c: " . mb_decode_numericentity('&#x00000000', $convmap), "\n";
echo "11d: " . mb_decode_numericentity('&#x10000', $convmap), "\n";

// Large decimal entity, converting from non-ASCII input encoding
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

test("10 digits for decimal entity", "&#0000000065;", "A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("More than 10 digits for decimal entity", "&#00000000165;", "&#00000000165;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("8 digits for hex entity", "&#x00000041;", "A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("More than 8 digits for hex entity", "&#x000000141;", "&#x000000141;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("Single &", "&", "&", [0, 0xFFFF, 0, 0xFFFF], "ASCII");

// We don't allow an entity to come right after a preceding ampersand
// (This is for compatibility with the legacy behavior of mb_decode_numericentity)
test("Successive &", "&&#2,", "&&#2,", [0xffe9ade7, 0x6237b6ff, 0xaa597469, 0x612800], 'ASCII');

// We don't allow an entity to come right after a preceding &#
// (Also for compatibility with the legacy behavior of mb_decode_numericentity)
test("Successive &#", "&#&#x32;", "&#&#x32;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Successive &#x", "&#x&#x32;", "&#x&#x32;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// Don't allow the starting & of an entity to terminate a preceding entity
// (Also for compatibility with the legacy behavior of mb_decode_numericentity)
test("Successive &#65", "&#65&#65;", "A&#65;", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// An entity CAN come right after an entity which is invalid because of being too long
test("Starting entity immediately after decimal entity which is too long", "&#10000000000&#65;", "&#10000000000A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Starting entity immediately after hex entity which is too long", "&#x111111111&#65;", "&#x111111111A", [0, 0xFFFF, 0, 0xFFFF], 'ASCII');

// The second entity is not accepted here, because it terminates a preceding entity
// (To test entities which are as large as possible without being too large, we need to use UCS-4;
// any other encoding would not allow codepoints that large)
$ucs4_test1 = mb_convert_encoding("&#1000000000&#65;", 'UCS-4BE', 'ASCII');
testNonAscii("Starting entity immediately after valid decimal entity which is just within maximum length", $ucs4_test1, "\x3B\x9A\xCA\x00\x00\x00\x00&\x00\x00\x00#\x00\x00\x006\x00\x00\x005\x00\x00\x00;", [0, 0xFFFFFFFF, 0, 0xFFFFFFFF], 'UCS-4BE');
$ucs4_test2 = mb_convert_encoding("&#x11111111&#65;", 'UCS-4BE', 'ASCII');
testNonAscii("Starting entity immediately after valid hex entity which is just within maximum length",  $ucs4_test2, "\x11\x11\x11\x11\x00\x00\x00&\x00\x00\x00#\x00\x00\x006\x00\x00\x005\x00\x00\x00;", [0, 0xFFFFFFFF, 0, 0xFFFFFFFF], 'UCS-4BE');

test("Starting entity immediately after invalid decimal entity", "&#0&#65;", "&#0&#65;", [0x1, 0xFFFF, 0, 0xFFFF], 'ASCII');
test("Starting entity immediately after invalid hex entity", "&#x0&#65;", "&#x0&#65;", [0x1, 0xFFFF, 0, 0xFFFF], 'ASCII');

test("Starting entity immediately after too-big decimal entity", "&#7001492542&#65;", "&#7001492542A", [0, 0xFFFFFFFF, 0, 0xFFFFFFFF], 'ASCII');

// If the numeric entity decodes to 0xFFFFFFFF, that should be passed through
// Originally, the new implementation of mb_decode_numericentity used -1 as a marker indicating
// that the entity could not be successfully decoded, so if the entity decoded successfully to
// 0xFFFFFFFF (-1), it would be treated as an invalid entity
test("Regression test (entity which decodes to 0xFFFFFFFF)", "&#xe;", "?", [0xFFFFFF86, 0xFFFFFFFF, 0xF, 0xFC015448], 'HZ');

// With the legacy conversion filters, a trailing & could be truncated by mb_decode_numericentity,
// because some text encodings did not properly invoke the next flush function in the chain
test("Regression test (truncation of successive & with JIS encoding)", "&&&", "&&&", [0x20FF37FF, 0x7202F569, 0xC4090023, 0xF160], "JIS");

// Previously, signed arithmetic was used on convmap entries
test("Regression test (convmap entries are now treated as unsigned)", "&#7,", "?,", [0x22FFFF11, 0xBF111189, 0x67726511, 0x1161E719], "ASCII");

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
11b: &#x0000000
11c: &#x00000000
11d: &#x10000
12: 00bc614e
13: f&ouml;o
14: mb_decode_numericentity(): Argument #2 ($map) must have a multiple of 4 elements
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
Successive &: string(5) "&&#2," => string(5) "&&#2," (Good)
Successive &#: string(8) "&#&#x32;" => string(8) "&#&#x32;" (Good)
Successive &#x: string(9) "&#x&#x32;" => string(9) "&#x&#x32;" (Good)
Successive &#65: string(9) "&#65&#65;" => string(6) "A&#65;" (Good)
Starting entity immediately after decimal entity which is too long: string(18) "&#10000000000&#65;" => string(14) "&#10000000000A" (Good)
Starting entity immediately after hex entity which is too long: string(17) "&#x111111111&#65;" => string(13) "&#x111111111A" (Good)
Starting entity immediately after valid decimal entity which is just within maximum length: 000000260000002300000031000000300000003000000030000000300000003000000030000000300000003000000030000000260000002300000036000000350000003b => 3b9aca00000000260000002300000036000000350000003b (Good)
Starting entity immediately after valid hex entity which is just within maximum length: 0000002600000023000000780000003100000031000000310000003100000031000000310000003100000031000000260000002300000036000000350000003b => 11111111000000260000002300000036000000350000003b (Good)
Starting entity immediately after invalid decimal entity: string(8) "&#0&#65;" => string(8) "&#0&#65;" (Good)
Starting entity immediately after invalid hex entity: string(9) "&#x0&#65;" => string(9) "&#x0&#65;" (Good)
Starting entity immediately after too-big decimal entity: string(17) "&#7001492542&#65;" => string(13) "&#7001492542A" (Good)
Regression test (entity which decodes to 0xFFFFFFFF): string(5) "&#xe;" => string(1) "?" (Good)
Regression test (truncation of successive & with JIS encoding): string(3) "&&&" => string(3) "&&&" (Good)
Regression test (convmap entries are now treated as unsigned): string(4) "&#7," => string(2) "?," (Good)
