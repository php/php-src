--TEST--
Exhaustive test of EUC-JP-2004 encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(200); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

$validChars = array(); /* EUC-JP-2004 string -> UTF-32BE */
$fromUnicode = array(); /* UTF-16BE -> EUC-JP-2004 */
$fp = fopen(__DIR__ . '/data/EUC-JP-2004.txt', 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  $codepoint2 = null;
  if (sscanf($line, "0x%x\tU+%x+%x", $bytes, $codepoint1, $codepoint2) >= 2) {
    if ($bytes < 256)
      $eucjp = chr($bytes);
    else if ($bytes <= 0xFFFF)
      $eucjp = pack('n', $bytes);
    else
      $eucjp = chr($bytes >> 16) . pack('n', $bytes & 0xFFFF);

    if ($codepoint2) {
      $validChars[$eucjp] = pack('NN', $codepoint1, $codepoint2);
    } else {
      $validChars[$eucjp] = pack('N', $codepoint1);
      if ($codepoint1 <= 0xFFFF)
        $fromUnicode[pack('n', $codepoint1)] = $eucjp;
    }
  }
}

/* Convert 0xA1B1 to U+FFE3 (FULLWIDTH MACRON), not U+203E (OVERLINE) */
$validChars["\xA1\xB1"] = "\x00\x00\xFF\xE3";
$fromUnicode["\xFF\xE3"] = "\xA1\xB1";

/* Convert 0xA1EF to U+FFE5 (FULLWIDTH YEN SIGN), not U+00A5 (YEN SIGN) */
$validChars["\xA1\xEF"] = "\x00\x00\xFF\xE5";
$fromUnicode["\xFF\xE5"] = "\xA1\xEF";

/* Convert U+00A5 (YEN SIGN) to 0x5C; that is one of the single bytes
 * which many legacy Japanese text encodings used to represent something
 * different from its normal meaning ASCII. In ASCII it's a backslash,
 * but legacy Japanese software often used it for a yen sign. */
$fromUnicode["\x00\xA5"] = "\x5C";
/* The other one is 0x7E, which is a tilde in ASCII, but was used in
 * legacy Japanese software for an overline */
$fromUnicode["\x20\x3E"] = "\x7E";

testAllValidChars($validChars, 'EUC-JP-2004', 'UTF-32BE');
echo "EUC-JP-2004 verification and conversion works for all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated);
testAllInvalidChars($invalidChars, $validChars, 'EUC-JP-2004', 'UTF-32BE', "\x00\x00\x00%");
testTruncatedChars($truncated, 'EUC-JP-2004', 'UTF-32BE', "\x00\x00\x00%");
echo "EUC-JP-2004 verification and conversion rejects all invalid characters\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'EUC-JP-2004', false);
echo "Unicode -> EUC-JP-2004 conversion works on all valid characters\n";

findInvalidChars($fromUnicode, $invalidChars, $unused, array_fill_keys(range(0, 0xFF), 2));
convertAllInvalidChars($invalidChars, $fromUnicode, 'UTF-16BE', 'EUC-JP-2004', '%');
echo "Unicode -> EUC-JP-2004 conversion works on all invalid characters\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "EUC-JP-2004", "UTF-8");
convertInvalidString("\xFE\xFF", "%", "EUC-JP-2004", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
EUC-JP-2004 verification and conversion works for all valid characters
EUC-JP-2004 verification and conversion rejects all invalid characters
Unicode -> EUC-JP-2004 conversion works on all valid characters
Unicode -> EUC-JP-2004 conversion works on all invalid characters
Done!
