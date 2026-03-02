--TEST--
Exhaustive test of Shift-JIS encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
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

/* According to the relevant Japan Industrial Standards Committee standards,
 * SJIS 0x5C is a Yen sign, and 0x7E is an overline.
 *
 * However, this conflicts with the implementation of SJIS in various legacy
 * software (notably Microsoft products), where SJIS 0x5C and 0x7E are taken
 * as equivalent to the same ASCII bytes.
 *
 * Prior to PHP 8.1, mbstring's implementation of SJIS handled these bytes
 * compatibly with Microsoft products. This was changed in PHP 8.1.0, in an
 * attempt to comply with the JISC specifications. However, after discussion
 * with various concerned Japanese developers, it seems that the historical
 * behavior was more useful in the majority of applications which process
 * SJIS-encoded text. */
$validChars["\x5C"] = "\x00\x5C";
$validChars["\x7E"] = "\x00\x7E";
$fromUnicode["\x00\x5C"] = "\x5C";
$fromUnicode["\x00\x7E"] = "\x7E";

/* That means it does not make sense to convert U+203E (OVERLINE)
 * to 0x7E; convert it to JIS X 0208 FULLWIDTH MACRON instead */
$fromUnicode["\x20\x3E"] = "\x81\x50";
/* U+00AF is MACRON; convert that to FULLWIDTH MACRON as well */
$fromUnicode["\x00\xAF"] = "\x81\x50";
/* Since we are treating 0x5C as equivalent to U+005C, it does not
 * make sense to convert U+00A5 (YEN SIGN) to 0x5C
 * Convert it to JIS X 0208 FULLWIDTH YEN SIGN instead */
$fromUnicode["\x00\xA5"] = "\x81\x8F";

/* DEL character */
$validChars["\x7F"] = "\x00\x7F";
$fromUnicode["\x00\x7F"] = "\x7F";
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

testValidString("\xFF\x5E", "\x81\x60", 'UTF-16BE', 'SJIS', false);
echo "Other mappings from Unicode -> SJIS are OK\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "Shift-JIS", "UTF-8");
convertInvalidString("\x81\x20", "%", "Shift-JIS", "UTF-8");
convertInvalidString("\xEA\xA9", "%", "Shift-JIS", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
SJIS verification and conversion works on all valid characters
SJIS verification and conversion works on all invalid characters
Unicode -> SJIS conversion works on all valid characters
Unicode -> SJIS conversion works on all invalid characters
Other mappings from Unicode -> SJIS are OK
Done!
