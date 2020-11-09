--TEST--
Exhaustive test of Shift-JIS encoding verification and conversion
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(999); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in Shift-JIS */
readConversionTable(__DIR__ . '/data/SHIFTJIS.txt', $validChars, $fromUnicode);

for ($i = 0; $i < 0x20; $i++)
  $validChars[chr($i)] = "\x00" . chr($i);
$validChars["\x7F"] = "\x00\x7F"; /* DEL character */
/* Use fullwidth reverse solidus, not (halfwidth) backslash (0x5C) */
$validChars["\x81\x5F"] = "\xFF\x3C";

testAllValidChars($validChars, 'Shift-JIS', 'UTF-16BE');
echo "SJIS verification and conversion works on all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated,
  map(range(0x81, 0x9F), 2, map(range(0xE0, 0xEF), 2)));
testAllInvalidChars($invalidChars, $validChars, 'Shift-JIS', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'Shift-JIS', 'UTF-16BE', "\x00%");
echo "SJIS verification and conversion works on all invalid characters\n";

findInvalidChars($fromUnicode, $invalidChars, $unused, map(range(0, 0xFF), 2));
testAllInvalidChars($invalidChars, $fromUnicode, 'UTF-16BE', 'Shift-JIS', '%');
echo "Unicode -> SJIS conversion works on all invalid characters\n";
?>
--EXPECT--
SJIS verification and conversion works on all valid characters
SJIS verification and conversion works on all invalid characters
Unicode -> SJIS conversion works on all invalid characters
