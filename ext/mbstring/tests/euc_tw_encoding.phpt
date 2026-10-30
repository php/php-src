--TEST--
Exhaustive test of verification and conversion of EUC-TW text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_INT_SIZE == 4 && !extension_loaded("ctype")) die("skip needs ctype extension on 32-bit");
?>
--FILE--
<?php
include('encoding_tests.inc');

srand(1000); // Make results consistent
mb_substitute_character(0x25); // '%'
readConversionTable(__DIR__ . '/data/EUC-TW.txt', $toUnicode, $fromUnicode);

findInvalidChars($toUnicode, $invalid, $truncated);
testAllValidChars($toUnicode, 'EUC-TW', 'UTF-16BE', false);
testAllValidChars($fromUnicode, 'UTF-16BE', 'EUC-TW', false);
testAllInvalidChars($invalid, $toUnicode, 'EUC-TW', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'EUC-TW', 'UTF-16BE', "\x00%");
echo "Tested EUC-TW -> UTF-16BE\n";

findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'EUC-TW', '%');
echo "Tested UTF-16BE -> EUC-TW\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "EUC-TW", "UTF-8");
convertInvalidString("\x8E\x20", "%", "EUC-TW", "UTF-8");
convertInvalidString("\x8E\xA1\x20", "%", "EUC-TW", "UTF-8");
convertInvalidString("\x8E\xA1\xA1\x20", "%", "EUC-TW", "UTF-8");
convertInvalidString("\x8E\xA2\xA3\x20", "%", "EUC-TW", "UTF-8");
convertInvalidString("\x8F", "%", "EUC-TW", "UTF-8");
convertInvalidString("\xA1\x50", "%", "EUC-TW", "UTF-8");
convertInvalidString("\xFD\xCC", "%", "EUC-TW", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested EUC-TW -> UTF-16BE
Tested UTF-16BE -> EUC-TW
Done!
