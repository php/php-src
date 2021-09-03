--TEST--
Exhaustive test of CP51932 encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(2020); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in CP51932 */
$validChars = array(); /* CP51932 string -> UTF-16BE string */
$fromUnicode = array();

$fp = fopen(realpath(__DIR__ . '/data/CP51932.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  $byte2 = null;
  if (sscanf($line, '<U%x> \x%x\x%x', $codepoint, $byte1, $byte2) >= 2) {
    /* The table we are using tries to map as many Unicode codepoints into
     * CP51932 as possible, including by mapping latin characters with accents
     * to the equivalent without accents; but since CP51932 is based on the
     * CP932 character set, we don't need to handle codepoints which are not
     * mapped from any character in CP932 */
    if (($codepoint >= 0xC0 && $codepoint <= 0xD6) ||
        ($codepoint >= 0xD8 && $codepoint <= 0xF6) ||
        ($codepoint >= 0xF8 && $codepoint <= 0xFF))
      continue;
    $cp51932 = ($byte2 ? (chr($byte1) . chr($byte2)) : chr($byte1));
    $utf16 = pack('n', $codepoint);
    $validChars[$cp51932] = $utf16;
    $fromUnicode[$utf16] = $cp51932;
  }
}

/* We map the JIS X 0208 FULLWIDTH TILDE to U+FF5E (FULLWIDTH TILDE)
 * But when converting Unicode to CP51932, we also accept U+301C (WAVE DASH) */
$fromUnicode["\x30\x1C"] = "\xA1\xC1";
/* We map the JIS X 0208 MINUS SIGN to U+FF0D (FULLWIDTH HYPHEN-MINUS SIGN),
 * but when converting Unicode to CP51932, we also accept U+2212 (MINUS SIGN) */
$fromUnicode["\x22\x12"] = "\xA1\xDD";
/* We map the JIS X 0208 PARALLEL TO symbol to U+2225 (PARALLEL TO),
 * but when converting Unicode to CP51932, we also accept U+2016
 * (DOUBLE VERTICAL LINE) */
$fromUnicode["\x20\x16"] = "\xA1\xC2";

/* There are a number of duplicate, irreversible mappings in the CP51932 table
 * In most cases, the one which we primarily use appears last in the table,
 * but in some cases, it is first and will be overwritten in the above loop
 *
 * Interestingly, the "collisions" happen in both directions! Part of this is
 * because the table we are using attempts to map as many Unicode codepoints
 * as possible to CP932 characters */
$fromUnicode["\x22\x20"] = "\xA2\xDC";
$fromUnicode["\x22\x29"] = "\xA2\xC1";
$fromUnicode["\x22\x2B"] = "\xA2\xE9";
$fromUnicode["\x22\x35"] = "\xA2\xE8";
$fromUnicode["\x22\x1A"] = "\xA2\xE5";
$fromUnicode["\x22\x2A"] = "\xA2\xC0";
$fromUnicode["\x22\x61"] = "\xA2\xE1";
$fromUnicode["\x22\xA5"] = "\xA2\xDD";
$fromUnicode["\x22\x52"] = "\xA2\xE2";
$fromUnicode["\xFF\xE2"] = "\xA2\xCC";
unset($fromUnicode["\x00\xA1"]); // Don't map upside-down ! to ordinary !
unset($fromUnicode["\x00\xA6"]); // Don't map broken bar to ordinary pipe character
unset($fromUnicode["\x00\xA9"]); // Don't map © to c
unset($fromUnicode["\x00\xAA"]); // Don't map feminine ordinal indicator
unset($fromUnicode["\x00\xAB"]); // Don't map left double angled quote mark to "much less than"
unset($fromUnicode["\x00\xAD"]); // Don't map soft hyphen to ordinary hyphen
unset($fromUnicode["\x00\xAE"]); // Don't map ® to R
unset($fromUnicode["\x00\xAF"]); // Don't map Unicode halfwidth macron to CP932 fullwidth macron
unset($fromUnicode["\x00\xB2"]); // Don't map ² to ordinary 2
unset($fromUnicode["\x00\xB3"]); // Don't map ³ to ordinary 3
unset($fromUnicode["\x00\xB5"]); // Don't map micro sign to Greek mu
unset($fromUnicode["\x00\xB7"]); // Don't map middle dot to katakana middle dot
unset($fromUnicode["\x00\xB8"]); // Don't map cedilla to fullwidth comma
unset($fromUnicode["\x00\xB9"]); // Don't map ¹ to ordinary 1
unset($fromUnicode["\x00\xBA"]); // Don't map "masculine ordinal indicator"
unset($fromUnicode["\x00\xBB"]); // Don't map right double angled quote mark to "much greater than"
unset($fromUnicode["\x30\x94"]); // Don't map hiragana vu to katakana vu

for ($i = 0; $i <= 0x7F; $i++)
  $validChars[chr($i)] = "\x00" . chr($i);

/* U+00A5 is YEN SIGN; convert to FULLWIDTH YEN SIGN */
$fromUnicode["\x00\xA5"] = "\xA1\xEF";
/* U+203E is OVERLINE; convert to FULLWIDTH MACRON */
$fromUnicode["\x20\x3E"] = "\xA1\xB1";
/* U+00AF is MACRON; convert to FULLWIDTH MACRON */
$fromUnicode["\x00\xAF"] = "\xA1\xB1";

testAllValidChars($validChars, 'CP51932', 'UTF-16BE', false);
testAllValidChars($fromUnicode, 'UTF-16BE', 'CP51932', false);
echo "CP51932 verification and conversion works on all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated, array_fill_keys(range(0xA9, 0xAF), 2) + array_fill_keys(range(0xF5, 0xF8), 2) + array(0xFD => 2, 0xFE => 2));

testAllInvalidChars($invalidChars, $validChars, 'CP51932', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'CP51932', 'UTF-16BE', "\x00%");
echo "CP51932 verification and conversion works on all invalid characters\n";

findInvalidChars($fromUnicode, $invalidCodepoints, $unused, array_fill_keys(range(0, 0xFF), 2));
convertAllInvalidChars($invalidCodepoints, $fromUnicode, 'UTF-16BE', 'CP51932', '%');
echo "Unicode -> CP51932 conversion works on all invalid codepoints\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "CP51932", "UTF-8");
convertInvalidString("\xFE\xFF", "%", "CP51932", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
CP51932 verification and conversion works on all valid characters
CP51932 verification and conversion works on all invalid characters
Unicode -> CP51932 conversion works on all invalid codepoints
Done!
