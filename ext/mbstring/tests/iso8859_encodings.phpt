--TEST--
Exhaustive test of verification and conversion of ISO-8859-X text
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(678); // Make results consistent
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

for ($n = 1; $n <= 16; $n++) {
    if ($n == 11 || $n == 12)
        continue;

    readConversionTable(realpath(__DIR__ . "/data/8859-$n.txt"), $toUnicode, $fromUnicode);
    findInvalidChars($toUnicode, $invalid, $unused);
    testAllValidChars($toUnicode, "ISO-8859-{$n}", 'UTF-16BE');
    testAllInvalidChars($invalid, $toUnicode, "ISO-8859-{$n}", 'UTF-16BE', "\x00%");

    findInvalidChars($fromUnicode, $invalid, $truncated, map(range(0,0xFF), 2));
    testAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', "ISO-8859-{$n}", '%');
    testTruncatedChars($truncated, 'UTF-16BE', "ISO-8859-{$n}", '%');

    echo "All is fine and dandy for ISO-8859-$n text\n";
}
?>
--EXPECT--
All is fine and dandy for ISO-8859-1 text
All is fine and dandy for ISO-8859-2 text
All is fine and dandy for ISO-8859-3 text
All is fine and dandy for ISO-8859-4 text
All is fine and dandy for ISO-8859-5 text
All is fine and dandy for ISO-8859-6 text
All is fine and dandy for ISO-8859-7 text
All is fine and dandy for ISO-8859-8 text
All is fine and dandy for ISO-8859-9 text
All is fine and dandy for ISO-8859-10 text
All is fine and dandy for ISO-8859-13 text
All is fine and dandy for ISO-8859-14 text
All is fine and dandy for ISO-8859-15 text
All is fine and dandy for ISO-8859-16 text
