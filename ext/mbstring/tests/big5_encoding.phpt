--TEST--
Exhaustive test of verification and conversion of Big5 text
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
readConversionTable(__DIR__ . '/data/BIG5.txt', $toUnicode, $fromUnicode);

// Non-reversible mappings (two different BIG5 codepoints map to same Unicode codepoint)
$fromUnicode["\x25\x50"] = "\xA2\xA4";
$fromUnicode["\x25\x5E"] = "\xA2\xA5";
$fromUnicode["\x25\x61"] = "\xA2\xA7";
$fromUnicode["\x25\x6A"] = "\xA2\xA6";
$fromUnicode["\x25\x6D"] = "\xA2\x7E";
$fromUnicode["\x25\x6E"] = "\xA2\xA1";
$fromUnicode["\x25\x6F"] = "\xA2\xA3";
$fromUnicode["\x25\x70"] = "\xA2\xA2";

findInvalidChars($toUnicode, $invalid, $truncated);
testAllValidChars($toUnicode, 'BIG5', 'UTF-16BE', false);
testAllInvalidChars($invalid, $toUnicode, 'BIG5', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'BIG5', 'UTF-16BE', "\x00%");
echo "Tested BIG5 -> UTF-16BE\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'BIG5', false);
findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'BIG5', '%');
echo "Tested UTF-16BE -> BIG5\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "BIG5", "UTF-8");
convertInvalidString("\xB0\x9F", "%", "BIG5", "UTF-8");
convertInvalidString("\xA3\xED", "%", "BIG5", "UTF-8");
convertInvalidString("\x76\x54", "U+7654", "UTF-16BE", "BIG5");

echo "Done!\n";
?>
--EXPECT--
Tested BIG5 -> UTF-16BE
Tested UTF-16BE -> BIG5
Done!
