--TEST--
mb_decode_numericentity() with 0xFFFFFFFF in conversion map
--EXTENSIONS--
mbstring
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
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

$ucs4_test1 = mb_convert_encoding("&#1000000000&#65;", 'UCS-4BE', 'ASCII');
testNonAscii("Starting entity immediately after valid decimal entity which is just within maximum length", $ucs4_test1, "\x3B\x9A\xCA\x00\x00\x00\x00A", [0, 0xFFFFFFFF, 0, 0xFFFFFFFF], 'UCS-4BE');
$ucs4_test2 = mb_convert_encoding("&#x11111111&#65;", 'UCS-4BE', 'ASCII');
testNonAscii("Starting entity immediately after valid hex entity which is just within maximum length",  $ucs4_test2, "\x11\x11\x11\x11\x00\x00\x00A", [0, 0xFFFFFFFF, 0, 0xFFFFFFFF], 'UCS-4BE');

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
Starting entity immediately after valid decimal entity which is just within maximum length: 000000260000002300000031000000300000003000000030000000300000003000000030000000300000003000000030000000260000002300000036000000350000003b => 3b9aca0000000041 (Good)
Starting entity immediately after valid hex entity which is just within maximum length: 0000002600000023000000780000003100000031000000310000003100000031000000310000003100000031000000260000002300000036000000350000003b => 1111111100000041 (Good)
Starting entity immediately after too-big decimal entity: string(17) "&#7001492542&#65;" => string(13) "&#7001492542A" (Good)
Regression test (entity which decodes to 0xFFFFFFFF): string(5) "&#xe;" => string(1) "?" (Good)
Regression test (truncation of successive & with JIS encoding): string(3) "&&&" => string(3) "&&&" (Good)
Regression test (convmap entries are now treated as unsigned): string(4) "&#7," => string(2) "?," (Good)
