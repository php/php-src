--TEST--
Exhaustive test of CP932 encoding verification and conversion (including 'SJIS-win' variant)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(4321); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in CP932 */
readConversionTable(__DIR__ . '/data/CP932.txt', $validChars, $fromUnicode);

/* Aside from the characters in that table, we also support a 'user' area
 * from 0xF040-0xF9FC, which map to Unicode 'private' codepoints 0xE000-E757 */
$codepoint = 0xE000;
for ($i = 0xF0; $i <= 0xF9; $i++) {
	for ($j = 0x40; $j <= 0xFC; $j++) {
		if ($j == 0x7F)
			continue;
		$utf16 = pack('n', $codepoint);
		$cp932 = chr($i) . chr($j);
		$validChars[$cp932] = $utf16;
		$fromUnicode[$utf16] = $cp932;
		$codepoint++;
	}
}

/* U+00A2 is CENT SIGN; convert to FULLWIDTH CENT SIGN */
$fromUnicode["\x00\xA2"] = "\x81\x91";
/* U+00A3 is POUND SIGN; convert to FULLWIDTH POUND SIGN */
$fromUnicode["\x00\xA3"] = "\x81\x92";
/* U+00A5 is YEN SIGN; convert to 0x5C, which has conflicting uses
 * (either as backslash or as Yen sign) */
$fromUnicode["\x00\xA5"] = "\x5C";


/* We map the JIS X 0208 FULLWIDTH TILDE to U+FF5E (FULLWIDTH TILDE)
 * But when converting Unicode to CP932, we also accept U+301C (WAVE DASH) */
$fromUnicode["\x30\x1C"] = "\x81\x60";
/* We map the JIS X 0208 MINUS SIGN to U+FF0D (FULLWIDTH HYPHEN-MINUS SIGN),
 * but when converting Unicode to CP932, we also accept U+2212 (MINUS SIGN) */
$fromUnicode["\x22\x12"] = "\x81\x7C";
/* We map the JIS X 0208 PARALLEL TO symbol to U+2225 (PARALLEL TO),
 * but when converting Unicode to CP932, we also accept U+2016
 * (DOUBLE VERTICAL LINE) */
$fromUnicode["\x20\x16"] = "\x81\x61";
/* We map the JIS X 0208 NOT SIGN to U+FFE2 (FULLWIDTH NOT SIGN),
 * but when converting Unicode to CP932, we also accept U+00AC (NOT SIGN) */
$fromUnicode["\x00\xAC"] = "\x81\xCA";

/* U+00AF is MACRON; convert to FULLWIDTH MACRON */
$fromUnicode["\x00\xAF"] = "\x81\x50";

/* U+203E is OVERLINE; convert to 0x7E, which has conflicting uses
 * (either as tilde or as overline) */
$fromUnicode["\x20\x3E"] = "\x7E";

findInvalidChars($validChars, $invalidChars, $truncated, array_fill_keys(range(0x81, 0x9F), 2) + array_fill_keys(range(0xE0, 0xFC), 2));

findInvalidChars($fromUnicode, $invalidCodepoints, $unused, array_fill_keys(range(0, 0xFF), 2));

/* There are 396 Unicode codepoints which are non-invertible in CP932
 * (multiple CP932 byte sequences map to the same codepoint)
 * Some of these are 3-way pile-ups. I wonder what the fine folks at MS
 * were thinking when they designed this text encoding. */

/* Everything from 0xED00-0xEEFF falls in this unfortunate category
 * (Other sequences in 0xFA00-0xFBFF map to the same codepoints, and when
 * converting from Unicode back to CP932, we favor the F's rather than the E's) */
$nonInvertible = array();
for ($i = 0xED00; $i <= 0xEEFF; $i++) {
	$bytes = pack('n', $i);
	if (isset($validChars[$bytes])) {
		unset($fromUnicode[$validChars[$bytes]]);
		$nonInvertible[$bytes] = $validChars[$bytes];
		unset($validChars[$bytes]); // will test these separately
	}
}

/* There are 23 other collisions between 2-byte sequences which variously
 * start with 0x81, 0x87, or 0xFA
 * We _love_ 0x81 and use it when possible. 0x87 is a second favorite */
for ($i = 0xFA4A; $i <= 0xFA53; $i++) {
	$bytes = pack('n', $i);
	unset($fromUnicode[$validChars[$bytes]]);
	$nonInvertible[$bytes] = $validChars[$bytes];
	unset($validChars[$bytes]); // will test these separately
}
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C, 0xFA54, 0xFA58, 0xFA59, 0xFA5A, 0xFA5B] as $i) {
	$bytes = pack('n', $i);
	unset($fromUnicode[$validChars[$bytes]]);
	$nonInvertible[$bytes] = $validChars[$bytes];
	unset($validChars[$bytes]); // will test these separately
}

testAllValidChars($validChars, 'CP932', 'UTF-16BE');
foreach ($nonInvertible as $cp932 => $unicode)
	testValidString($cp932, $unicode, 'CP932', 'UTF-16BE', false);
echo "CP932 verification and conversion works on all valid characters\n";

testAllInvalidChars($invalidChars, $validChars, 'CP932', 'UTF-16BE', "\x00%");
echo "CP932 verification and conversion works on all invalid characters\n";

convertAllInvalidChars($invalidCodepoints, $fromUnicode, 'UTF-16BE', 'CP932', '%');
echo "Unicode -> CP932 conversion works on all invalid codepoints\n";

/* Now test 'SJIS-win' variant of CP932, which is really CP932 but with
 * two different mappings
 * Instead of mapping U+00A5 and U+203E to the single bytes 0x5C and 07E
 * (which have conflicting uses), 'SJIS-win' maps them to appropriate
 * JIS X 0208 characters */

/* U+00A5 is YEN SIGN; convert to FULLWIDTH YEN SIGN */
$fromUnicode["\x00\xA5"] = "\x81\x8F";
/* U+203E is OVERLINE; convert to JIS X 0208 FULLWIDTH MACRON */
$fromUnicode["\x20\x3E"] = "\x81\x50";

testAllValidChars($validChars, 'SJIS-win', 'UTF-16BE');
foreach ($nonInvertible as $cp932 => $unicode)
	testValidString($cp932, $unicode, 'SJIS-win', 'UTF-16BE', false);
echo "SJIS-win verification and conversion works on all valid characters\n";

testAllInvalidChars($invalidChars, $validChars, 'SJIS-win', 'UTF-16BE', "\x00%");
echo "SJIS-win verification and conversion works on all invalid characters\n";

convertAllInvalidChars($invalidCodepoints, $fromUnicode, 'UTF-16BE', 'SJIS-win', '%');
echo "Unicode -> SJIS-win conversion works on all invalid codepoints\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "CP932", "UTF-8");
convertInvalidString("\xEA", "%", "CP932", "UTF-8");
convertInvalidString("\x81\x20", "%", "CP932", "UTF-8");
convertInvalidString("\xEA\xA9", "%", "CP932", "UTF-8");
convertInvalidString("\x80", "%", "SJIS-win", "UTF-8");
convertInvalidString("\xEA", "%", "SJIS-win", "UTF-8");
convertInvalidString("\x81\x20", "%", "SJIS-win", "UTF-8");
convertInvalidString("\xEA\xA9", "%", "SJIS-win", "UTF-8");

echo 'mb_strlen("\x80\x81", "CP932") == ' . mb_strlen("\x80\x81", "CP932") . PHP_EOL;

echo "Done!\n";
?>
--EXPECT--
CP932 verification and conversion works on all valid characters
CP932 verification and conversion works on all invalid characters
Unicode -> CP932 conversion works on all invalid codepoints
SJIS-win verification and conversion works on all valid characters
SJIS-win verification and conversion works on all invalid characters
Unicode -> SJIS-win conversion works on all invalid codepoints
mb_strlen("\x80\x81", "CP932") == 2
Done!
