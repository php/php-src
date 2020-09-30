--TEST--
Exhaustive test of EUC-JP encoding verification and conversion
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
srand(555); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in EUC-JP */
$validChars = array(); /* EUC-JP string -> UTF-32BE string */
$fp = fopen(realpath(__DIR__ . '/../docs/EUC-JP.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
    if ($bytes < 256)
      $eucJP = chr($bytes);
    else if ($bytes <= 0xFFFF)
      $eucJP = pack('n', $bytes);
    else
      $eucJP = chr(($bytes >> 16) & 0xFF) . chr(($bytes >> 8) & 0xFF) . chr($bytes & 0xFF);

    $utf32 = pack('N', $codepoint);
    $validChars[$eucJP] = $utf32;
  }
}

/* The JIS X 0208 character set does not have a single, straightforward
 * mapping to the Unicode character set */

/* Kuten code 0x2140 (EUC-JP 0xA1C0) is a backslash; this can be mapped to
 * 0x005C for an ordinary backslash, or 0xFF3C for a _fullwidth_ one
 * We go with fullwidth */
$validChars["\xA1\xC0"] = "\x00\x00\xFF\x3C";

/* In the JIS X 0212 character set, kuten code 0x2237 (EUC-JP 0x8FA2B7)
 * is an ordinary tilde character
 * This mapping is not reversible, because ASCII 0x7E also represents
 * the same character */
unset($validChars["\x8F\xA2\xB7"]);

/* Go through all the possible ways that a EUC-JP character could be formed
 * in an invalid way */
$invalidChars = array();
for ($i = 0; $i < 256; $i++) {
  /* Look for bad single-byte chars */
  if (($i >= 0xA1 && $i <= 0xFE) || $i == 0x8E || $i == 0x8F)
    continue;
  if (!isset($validChars[chr($i)]))
    $invalidChars[chr($i)] = true;
}
for ($i = 0xA1; $i <= 0xFE; $i++) {
  for ($j = 0; $j < 256; $j++) {
    /* Bad double-byte, JIS X 0208 characters... */
    $char = chr($i) . chr($j);
    if (!isset($validChars[$char]))
      $invalidChars[$char] = true;
  }
}
for ($i = 0; $i < 256; $i++) {
  /* Also JIS X 0201... */
  $char = "\x8E" . chr($i);
  if (!isset($validChars[$char]))
    $invalidChars[$char] = true;
}
/* And finally cover all the ways to form an invalid JIS X 0212 character */
for ($i = 0; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xFE) {
    for ($j = 0; $j < 256; $j++) {
      $char = "\x8F" . chr($i) . chr($j);
      if ($j >= 0xA1 && $j <= 0xFE) {
        if (!isset($validChars[$char]))
          $invalidChars[$char] = true;
      } else {
        $invalidChars[$char] = true;
      }
    }
  } else {
    $invalidChars["\x8F" . chr($i)] = true;
  }
}

unset($invalidChars["\x8F\xA2\xB7"]);

testAllValidChars($validChars, 'EUC-JP', 'UTF-32BE');
echo "Encoding verification and conversion work for all valid characters\n";

testAllInvalidChars($invalidChars, $validChars, 'EUC-JP', 'UTF-32BE', "\x00\x00\x00%");
echo "Encoding verification and conversion work for all invalid characters\n";

testValidString("\x8F\xA2\xB7", "\x00\x00\x00~", 'EUC-JP', 'UTF-32BE', false);
echo "Irreversible mapping of 0x8FA2B7 follows JIS X 0212 correctly\n";

?>
--EXPECT--
Encoding verification and conversion work for all valid characters
Encoding verification and conversion work for all invalid characters
Irreversible mapping of 0x8FA2B7 follows JIS X 0212 correctly
