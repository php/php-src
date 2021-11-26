--TEST--
Exhaustive test of EUC-JP encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(555); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in EUC-JP */
readConversionTable(__DIR__ . '/data/EUC-JP.txt', $validChars, $fromUnicode, true);

/* The JIS X 0208 character set does not have a single, straightforward
 * mapping to the Unicode character set */

/* Kuten code 0x2140 (EUC-JP 0xA1C0) is a backslash; this can be mapped to
 * 0x005C for an ordinary backslash, or 0xFF3C for a _fullwidth_ one
 * We go with fullwidth */
$validChars["\xA1\xC0"] = "\x00\x00\xFF\x3C";
$fromUnicode["\x00\x00\xFF\x3C"] = "\xA1\xC0";

/* Unicode has both halfwidth and fullwidth NOT SIGN; convert both of them
 * to JIS X 0208 NOT SIGN */
$fromUnicode["\x00\x00\xFF\xE2"] = "\xA2\xCC";
/* Likewise for fullwidth and halfwidth POUND SIGN */
$fromUnicode["\x00\x00\xFF\xE1"] = "\xA1\xF2";
/* Likewise for fullwidth and halfwidth CENT SIGN */
$fromUnicode["\x00\x00\xFF\xE0"] = "\xA1\xF1";
/* Convert Unicode FULLWIDTH TILDE to JIS X 0208 WAVE DASH */
$fromUnicode["\x00\x00\xFF\x5E"] = "\xA1\xC1";
/* Convert Unicode FULLWIDTH HYPHEN-MINUS to JIS X 0208 MINUS SIGN */
$fromUnicode["\x00\x00\xFF\x0D"] = "\xA1\xDD";
/* Convert Unicode PARALLEL TO to JIS X 0208 DOUBLE VERTICAL LINE */
$fromUnicode["\x00\x00\x22\x25"] = "\xA1\xC2";

/* Unicode 0x007E (tilde) can be represented in two different ways in EUC-JP
 * When converting Unicode to EUC-JP, use the simpler representation */
$fromUnicode["\x00\x00\x00\x7E"] = "\x7E";
/* Likewise with 0x005C */
$fromUnicode["\x00\x00\x00\x5C"] = "\x5C";

/* U+203E is OVERLINE; convert to FULLWIDTH MACRON */
$fromUnicode["\x00\x00\x20\x3E"] = "\xA1\xB1";

findInvalidChars($validChars, $invalidChars, $truncated, array_fill_keys(range(0xA1, 0xFE), 2) + array(0x8E => 2, 0x8F => 3));

/* In the JIS X 0212 character set, kuten code 0x2237 (EUC-JP 0x8FA2B7)
 * is an ordinary tilde character
 * This mapping is not reversible, because ASCII 0x7E also represents
 * the same character */
unset($validChars["\x8F\xA2\xB7"]);

testAllValidChars($validChars, 'EUC-JP', 'UTF-32BE');
echo "Encoding verification and conversion work for all valid characters\n";

testAllInvalidChars($invalidChars, $validChars, 'EUC-JP', 'UTF-32BE', "\x00\x00\x00%");
testTruncatedChars($truncated, 'EUC-JP', 'UTF-32BE', "\x00\x00\x00%");
echo "Encoding verification and conversion work for all invalid characters\n";

testValidString("\x8F\xA2\xB7", "\x00\x00\x00~", 'EUC-JP', 'UTF-32BE', false);
echo "Irreversible mapping of 0x8FA2B7 follows JIS X 0212 correctly\n";

testAllValidChars($fromUnicode, 'UTF-32BE', 'EUC-JP', false);
echo "Unicode -> EUC-JP conversion works on all valid characters\n";

$invalidChars = array();
for ($cp = 0; $cp <= 0xFFFF; $cp++) {
  $char = pack('N', $cp);
  if (!isset($fromUnicode[$char]))
    $invalidChars[$char] = true;
}
convertAllInvalidChars($invalidChars, $fromUnicode, 'UTF-32BE', 'EUC-JP', '%');
echo "Unicode -> EUC-JP conversion works on all invalid characters\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "EUC-JP", "UTF-8");
convertInvalidString("\xFE\xFF", "%", "EUC-JP", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Encoding verification and conversion work for all valid characters
Encoding verification and conversion work for all invalid characters
Irreversible mapping of 0x8FA2B7 follows JIS X 0212 correctly
Unicode -> EUC-JP conversion works on all valid characters
Unicode -> EUC-JP conversion works on all invalid characters
Done!
