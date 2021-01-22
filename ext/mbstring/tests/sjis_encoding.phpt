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

for ($i = 0; $i < 0x20; $i++) {
  $validChars[chr($i)] = "\x00" . chr($i);
  $fromUnicode["\x00" . chr($i)] = chr($i);
}

/* U+007E is TILDE; convert to Shift-JIS 0x8160 (WAVE DASH) */
$fromUnicode["\x00\x7E"] = "\x81\x60";
/* DEL character */
$validChars["\x7F"] = "\x00\x7F";
$fromUnicode["\x00\x7F"] = "\x7F";
/* U+00AF is MACRON; Shift-JIS 0x7E is overline */
$fromUnicode["\x00\xAF"] = "\x7E";
/* Use fullwidth reverse solidus, not (halfwidth) backslash (0x5C) */
$validChars["\x81\x5F"] = "\xFF\x3C";
$fromUnicode["\xFF\x3C"] = "\x81\x5F";
/* Unicode has both halfwidth and fullwidth NOT SIGN; convert both of them
 * to JIS X 0208 NOT SIGN */
$fromUnicode["\xFF\xE2"] = "\x81\xCA";
/* Likewise for fullwidth and halfwidth POUND SIGN */
$fromUnicode["\xFF\xE1"] = "\x81\x92";
/* Likewise for fullwidth and halfwidth CENT SIGN */
$fromUnicode["\xFF\xE0"] = "\x81\x91";
/* Convert Unicode FULLWIDTH TILDE to JIS X 0208 WAVE DASH */
$fromUnicode["\xFF\x5E"] = "\x81\x60";
/* Convert Unicode FULLWIDTH HYPHEN-MINUS to JIS X 0208 MINUS SIGN */
$fromUnicode["\xFF\x0D"] = "\x81\x7C";
/* Convert Unicode PARALLEL TO to JIS X 0208 DOUBLE VERTICAL LINE */
$fromUnicode["\x22\x25"] = "\x81\x61";

testAllValidChars($validChars, 'Shift-JIS', 'UTF-16BE');
echo "SJIS verification and conversion works on all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated,
  array_fill_keys(range(0x81, 0x9F), 2) + array_fill_keys(range(0xE0, 0xEF), 2));
testAllInvalidChars($invalidChars, $validChars, 'Shift-JIS', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'Shift-JIS', 'UTF-16BE', "\x00%");
echo "SJIS verification and conversion works on all invalid characters\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'Shift-JIS', false);
echo "Unicode -> SJIS conversion works on all valid characters\n";

findInvalidChars($fromUnicode, $invalidChars, $unused, array_fill_keys(range(0, 0xFF), 2));
convertAllInvalidChars($invalidChars, $fromUnicode, 'UTF-16BE', 'Shift-JIS', '%');
echo "Unicode -> SJIS conversion works on all invalid characters\n";
?>
--EXPECT--
SJIS verification and conversion works on all valid characters
SJIS verification and conversion works on all invalid characters
Unicode -> SJIS conversion works on all valid characters
Unicode -> SJIS conversion works on all invalid characters
