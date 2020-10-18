--TEST--
Exhaustive test of verification and conversion of CP1252 text
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(789); // Make results consistent
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

readConversionTable(realpath(__DIR__ . '/data/CP1252.txt'), $toUnicode, $fromUnicode);
findInvalidChars($toUnicode, $invalid, $unused);
testAllValidChars($toUnicode, 'CP1252', 'UTF-16BE');
testAllInvalidChars($invalid, $toUnicode, 'CP1252', 'UTF-16BE', "\x00%");

findInvalidChars($fromUnicode, $invalid, $truncated, map(range(0,0xFF), 2));
testAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'CP1252', '%');
testTruncatedChars($truncated, 'UTF-16BE', 'CP1252', '%');

echo "All is fine and dandy for CP1252 text\n";
?>
--EXPECT--
All is fine and dandy for CP1252 text
