--TEST--
Exhaustive test of verification and conversion of ARMSCII-8 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
srand(111); // Make results consistent
mb_substitute_character(0x25); // '%'

readConversionTable(__DIR__ . '/data/ARMSCII-8.txt', $toUnicode, $fromUnicode);
$irreversible = ["\x28", "\x29", "\x2C", "\x2D", "\x2E"];

findInvalidChars($toUnicode, $invalid, $truncated);
testAllValidChars($toUnicode, 'ARMSCII-8', 'UTF-16BE', false);
foreach ($irreversible as $char)
  unset($toUnicode[$char]);
testAllValidChars($toUnicode, 'ARMSCII-8', 'UTF-16BE');
testAllInvalidChars($invalid, $toUnicode, 'ARMSCII-8', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'ARMSCII-8', 'UTF-16BE', "\x00%");
echo "Tested ARMSCII-8 -> UTF-16BE\n";

findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'ARMSCII-8', '%');
echo "Tested UTF-16BE -> ARMSCII-8\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\xA1", "%", "ARMSCII-8", "UTF-8");
convertInvalidString("\xFF", "%", "ARMSCII-8", "UTF-8");

// Test replacement character which cannot be encoded in ARMSCII-8
mb_substitute_character(0x1234);
convertInvalidString("\x23\x45", '?', 'UTF-16BE', 'ARMSCII-8');

echo "Done!\n";
?>
--EXPECT--
Tested ARMSCII-8 -> UTF-16BE
Tested UTF-16BE -> ARMSCII-8
Done!
