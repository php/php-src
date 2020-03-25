--TEST--
htmlentities() / htmlspecialchars() ENT_DISALLOWED
--FILE--
<?php
function codepoint_to_utf8($k) {
    if ($k < 0x80) {
        $retval = pack('C', $k);
    } else if ($k < 0x800) {
        $retval = pack('C2',
            0xc0 | ($k >> 6),
            0x80 | ($k & 0x3f));
    } else if ($k < 0x10000) {
        $retval = pack('C3',
            0xe0 | ($k >> 12),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    } else {
        $retval = pack('C4',
            0xf0 | ($k >> 18),
            0x80 | (($k >> 12) & 0x3f),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    }
    return $retval;
}

$tests = array(
    0x00, //C0
    0x01,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x1F,
    0x20, //allowed always
    0x7F, //DEL
    0x80, //C1
    0x9F,
    0xA0, //allowed always
    0xD7FF, //surrogates
    0xD800,
    0xDFFF,
    0xE000, //allowed always
    0xFFFE, //nonchar
    0xFFFF,
    0xFDCF, //allowed always
    0xFDD0, //nonchar
    0xFDEF,
    0xFDF0, //allowed always
    0x2FFFE, //nonchar
    0x2FFFF,
);
$tests2 = array_map('codepoint_to_utf8', $tests);

$subchr = codepoint_to_utf8(0xFFFD);

function test($flag) {
    global $tests, $tests2;
    $i = -1;
    foreach ($tests2 as $test) {
        $i++;
        $a = htmlentities($test, $flag | ENT_DISALLOWED, "UTF-8");
        $b = htmlspecialchars($test, $flag | ENT_DISALLOWED, "UTF-8");
        if ($a == "" && $b == "") { echo sprintf("%05X", $tests[$i]), ": INVALID SEQUENCE\n"; continue; }
        echo sprintf("%05X", $tests[$i]), ": ", bin2hex($a), " ", bin2hex($b), "\n";
    }
}

echo "*** Testing HTML 4.01 ***\n";

test(ENT_HTML401);

echo "\n*** Testing XHTML 1.0 ***\n";

test(ENT_XHTML);

echo "\n*** Testing HTML 5 ***\n";

test(ENT_HTML5);

echo "\n*** Testing XML 1.0 ***\n";

test(ENT_XML1);

?>
--EXPECT--
*** Testing HTML 4.01 ***
00000: efbfbd efbfbd
00001: efbfbd efbfbd
00009: 09 09
0000A: 0a 0a
0000B: efbfbd efbfbd
0000C: efbfbd efbfbd
0000D: 0d 0d
0000E: efbfbd efbfbd
0001F: efbfbd efbfbd
00020: 20 20
0007F: efbfbd efbfbd
00080: efbfbd efbfbd
0009F: efbfbd efbfbd
000A0: 266e6273703b c2a0
0D7FF: ed9fbf ed9fbf
0D800: INVALID SEQUENCE
0DFFF: INVALID SEQUENCE
0E000: ee8080 ee8080
0FFFE: efbfbe efbfbe
0FFFF: efbfbf efbfbf
0FDCF: efb78f efb78f
0FDD0: efb790 efb790
0FDEF: efb7af efb7af
0FDF0: efb7b0 efb7b0
2FFFE: f0afbfbe f0afbfbe
2FFFF: f0afbfbf f0afbfbf

*** Testing XHTML 1.0 ***
00000: efbfbd efbfbd
00001: efbfbd efbfbd
00009: 09 09
0000A: 0a 0a
0000B: efbfbd efbfbd
0000C: efbfbd efbfbd
0000D: 0d 0d
0000E: efbfbd efbfbd
0001F: efbfbd efbfbd
00020: 20 20
0007F: 7f 7f
00080: c280 c280
0009F: c29f c29f
000A0: 266e6273703b c2a0
0D7FF: ed9fbf ed9fbf
0D800: INVALID SEQUENCE
0DFFF: INVALID SEQUENCE
0E000: ee8080 ee8080
0FFFE: efbfbd efbfbd
0FFFF: efbfbd efbfbd
0FDCF: efb78f efb78f
0FDD0: efb790 efb790
0FDEF: efb7af efb7af
0FDF0: efb7b0 efb7b0
2FFFE: f0afbfbe f0afbfbe
2FFFF: f0afbfbf f0afbfbf

*** Testing HTML 5 ***
00000: efbfbd efbfbd
00001: efbfbd efbfbd
00009: 265461623b 09
0000A: 264e65774c696e653b 0a
0000B: efbfbd efbfbd
0000C: 0c 0c
0000D: 0d 0d
0000E: efbfbd efbfbd
0001F: efbfbd efbfbd
00020: 20 20
0007F: efbfbd efbfbd
00080: efbfbd efbfbd
0009F: efbfbd efbfbd
000A0: 266e6273703b c2a0
0D7FF: ed9fbf ed9fbf
0D800: INVALID SEQUENCE
0DFFF: INVALID SEQUENCE
0E000: ee8080 ee8080
0FFFE: efbfbd efbfbd
0FFFF: efbfbd efbfbd
0FDCF: efb78f efb78f
0FDD0: efbfbd efbfbd
0FDEF: efbfbd efbfbd
0FDF0: efb7b0 efb7b0
2FFFE: efbfbd efbfbd
2FFFF: efbfbd efbfbd

*** Testing XML 1.0 ***
00000: efbfbd efbfbd
00001: efbfbd efbfbd
00009: 09 09
0000A: 0a 0a
0000B: efbfbd efbfbd
0000C: efbfbd efbfbd
0000D: 0d 0d
0000E: efbfbd efbfbd
0001F: efbfbd efbfbd
00020: 20 20
0007F: 7f 7f
00080: c280 c280
0009F: c29f c29f
000A0: c2a0 c2a0
0D7FF: ed9fbf ed9fbf
0D800: INVALID SEQUENCE
0DFFF: INVALID SEQUENCE
0E000: ee8080 ee8080
0FFFE: efbfbd efbfbd
0FFFF: efbfbd efbfbd
0FDCF: efb78f efb78f
0FDD0: efb790 efb790
0FDEF: efb7af efb7af
0FDF0: efb7b0 efb7b0
2FFFE: f0afbfbe f0afbfbe
2FFFF: f0afbfbf f0afbfbf
