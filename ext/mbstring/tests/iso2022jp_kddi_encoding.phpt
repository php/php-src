--TEST--
Exhaustive test of ISO-2022-JP-KDDI text encoding
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(390); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

function shiftJISDecode($bytes) {
  /* Convert CP932's default Shift-JIS representation to kuten code */
  $first = ($bytes >> 8) & 0xFF;
  $second = $bytes & 0xFF;
  $hi_bits = $first - (($first > 0x9F) ? 0xE0 - 31 : 0x81);
  if ($second > 0x9E) {
    $kuten = ((($hi_bits << 1) + 0x22) << 8) + ($second - 0x9F + 0x21);
  } else if ($second > 0x7F) {
    $kuten = ((($hi_bits << 1) + 0x21) << 8) + ($second - 0x80 + 63 + 0x21);
  } else {
    $kuten = ((($hi_bits << 1) + 0x21) << 8) + ($second - 0x40 + 0x21);
  }
  return $kuten;
}

/* Read in the table of all characters in CP932 */
$cp932Chars = array(); /* CP932 string -> UTF-32BE string */
$fp = fopen(realpath(__DIR__ . '/data/CP932.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
    if ($bytes < 256)
      continue;
    /* For ISO-2022-JP-KDDI, we only accept the first range of MicroSoft
     * vendor extensions, in ku 13 */
    if ($bytes > 0xEAA4)
      continue;
    $cp932Chars[pack('n', shiftJISDecode($bytes))] = pack('N', $codepoint);
  }
}

/* Add KDDI-specific emoji to the CP932 characters
 * They are mapped in 22 ku (or 'rows') above the places where they are mapped
 * in the Shift-JIS representation of KDDI emoji */
$fp = fopen(realpath(__DIR__ . '/data/EmojiSources.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;
  $fields = explode(';', rtrim($line));
  if (count($fields) >= 4) {
    if (sscanf($fields[0], "%x %x", $cp1, $cp2) == 2)
      $utf32 = pack('N', $cp1) . pack('N', $cp2);
    else
      $utf32 = pack('N', hexdec($fields[0]));

    if ($fields[2]) {
      $kuten = shiftJISDecode(hexdec($fields[2]));
      $ku = $kuten >> 8;
      if ($ku >= 106 && $ku <= 112)
        $cp932Chars[pack('n', $kuten - (22 * 0x100))] = $utf32;
    }
  }
}

/* Duplicate mappings for the same characters in CP932 */
$nonInvertible = array();
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C] as $i) {
  $bytes = pack('n', shiftJISDecode($i));
  $nonInvertible[$bytes] = $cp932Chars[$bytes];
}

/* Read in table of all characters in JISX-0201 charset */
$jisx0201Chars = array(); /* JISX0201 -> UTF-32BE */
$fp = fopen(realpath(__DIR__ . '/data/JISX0201.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $byte, $codepoint) == 2)
    $jisx0201Chars[chr($byte)] = pack('N', $codepoint);
}

function testValid($from, $to, $encoding, $bothWays = true) {
  identifyValidString($from, $encoding);
  convertValidString($from, $to, $encoding, 'UTF-32BE', false);

  if ($bothWays) {
    /* ESC ( B at the beginning is redundant, since ASCII mode is the default */
    if (substr($from, 0, 3) == "\x1B(B")
      $from = substr($from, 3, strlen($from) - 3);
    /* If the string switches to a different charset, it should switch back to
     * ASCII at the end */
    if (strpos($from, "\x1B\$B") !== false || strpos($from, "\x1B(I") !== false || strpos($from, "\x1B\$@") !== false || strpos($from, "\x1B\$(B") !== false)
      $from .= "\x1B(B";

    convertValidString($to, $from, 'UTF-32BE', $encoding, false);
  }
}

function testInvalid($from, $to, $encoding) {
  testInvalidString($from, $to, $encoding, 'UTF-32BE');
}

for ($i = 0; $i < 0x80; $i++) {
  if ($i == 0x1B)
    continue;
  testValid(chr($i),            "\x00\x00\x00" . chr($i), 'ISO-2022-JP-KDDI');
  testValid("\x1B(B" . chr($i), "\x00\x00\x00" . chr($i), 'ISO-2022-JP-KDDI', false);
  testValid("\x1B(J" . chr($i), "\x00\x00\x00" . chr($i), 'ISO-2022-JP-KDDI', false);
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF) // We convert single bytes from 0xA1-0xDF as JIS X 0201 kana
    continue;
  testInvalid(chr($i),            "\x00\x00\x00%", 'ISO-2022-JP-KDDI');
  testInvalid("\x1B(B" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-KDDI');
  testInvalid("\x1B(J" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-KDDI');
}

echo "ASCII support OK\n";

/* All valid JIS X 0201 characters
 * Those with a 1 in the high bit are JIS X 0201 kana */
foreach ($jisx0201Chars as $jisx0201 => $utf32BE) {
  if (ord($jisx0201) >= 128) {
    $kana = chr(ord($jisx0201) - 128);
    testValid("\x1B(I" . $kana, $utf32BE, 'ISO-2022-JP-KDDI', false);
    testValid($jisx0201, $utf32BE, 'ISO-2022-JP-KDDI', false);
  }
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF)
    continue;
  testInvalid("\x1B(I" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-KDDI');
  testInvalid("\x1B(J" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-KDDI');
}

echo "JIS X 0201 support OK\n";

$validChars = $cp932Chars;
/* We allow ASCII/JIS X 0201 characters to appear even in JIS X 0208 mode */
for ($i = 0; $i <= 0x7F; $i++)
  $validChars[chr($i)] = chr($i);
for ($i = 0xA1; $i <= 0xDF; $i++)
  $validChars[chr($i)] = $jisx0201Chars[chr($i)];
$lenTable = array_fill_keys(range(0xE0, 0xFC), 2) + array_fill_keys(range(0x81, 0x9F), 2);
findInvalidChars($validChars, $invalidChars, $truncatedChars, $lenTable);

foreach ($nonInvertible as $bytes => $char)
  unset($cp932Chars[$bytes]);

$good = array_keys($cp932Chars);
shuffle($good);
while (!empty($good)) {
  $length = min(rand(5,10), count($good));
  $from = $to = '';
  while ($length--) {
    $goodChar = array_pop($good);
    $from .= $goodChar;
    $to .= $cp932Chars[$goodChar];
  }
  testValid("\x1B\$B" . $from, $to, 'ISO-2022-JP-KDDI');
}

$good = array_keys($nonInvertible);
shuffle($good);
while (!empty($good)) {
  $length = min(rand(5,10), count($good));
  $from = $to = '';
  while ($length--) {
    $goodChar = array_pop($good);
    $from .= $goodChar;
    $to .= $nonInvertible[$goodChar];
  }
  testValid("\x1B\$B" . $from, $to, 'ISO-2022-JP-KDDI', false);
}

foreach (array_keys($invalidChars) as $invalid) {
  $firstByte = ord($invalid[0]);
  if (($firstByte > 0x80 && $firstByte < 0xA0) || $firstByte >= 0xE0) {
    /* The first byte of this 2-byte character will be rejected and result in % being sent
     * to the output. Then the second byte will do something else. It is easier to write the
     * test if we only check with the 1st byte. */
    testInvalidString("\x1B\$B" . $invalid[0], "\x00\x00\x00%", 'ISO-2022-JP-KDDI', 'UTF-32BE');
  } else {
    testInvalidString("\x1B\$B" . $invalid, "\x00\x00\x00%", 'ISO-2022-JP-KDDI', 'UTF-32BE');
  }
}
// Try Kanji which starts with a good byte, but the 2nd byte is junk
testInvalidString("\x1B\$B\x21\xFF", "%", 'ISO-2022-JP-KDDI', 'UTF-8');

foreach (array_keys($truncatedChars) as $truncated)
  testInvalidString("\x1B\$B" . $truncated, "\x00\x00\x00%", 'ISO-2022-JP-KDDI', 'UTF-32BE');

testValidString("\x1B\$B\x76\x27", "\x00\x01\xF1\xEF\x00\x01\xF1\xF5", 'ISO-2022-JP-KDDI', 'UTF-32BE', false); // Japan flag emoji
testValidString("\x00#\x20\xE3", "\x1B\$B\x71\x69\x1B(B", 'UTF-16BE', 'ISO-2022-JP-KDDI', false); // Phone key emoji

testValidString("\x1B\$(B\x21\x21", "\x30\x00", 'ISO-2022-JP-KDDI', 'UTF-16BE', false); // Try ESC $ ( B escape sequence

// Switch from JISX 0208 Kanji to ASCII
testValidString("\x30\x00\x00A", "\x1B\$B\x21\x21\x1B(BA", "UTF-16BE", "ISO-2022-JP-KDDI", false);
// Switch from JISX 0208 Kanji to JISX 0201 Kana
testValidString("\x30\x00\xFF\x67", "\x1B\$B\x21\x21\x1B(I'\x1B(B", "UTF-16BE", "ISO-2022-JP-KDDI", false);

/* Convert Unicode flag emoji to ISO-2022-JP-KDDI proprietary flag emoji
 * I am not able to confirm that the kuten codes we are using for these proprietary emoji are the right ones
 * (There doesn't seem to be any publically available reference, and I don't have a legacy KDDI device)
 *
 * However, the conversion does not work in the opposite direction; this is because of the test
 * `if (s >= (84 * 94) && s < (91 * 94))`, which the kuten code which we are using for flag emoji doesn't match
 * That test is inherited from the old implementation (from libmbfl), and I have no way to confirm that
 * changing it won't break anything */
testValidString("\x00\x01\xF1\xF0\x00\x01\xF1\xF7", "\x1B\$B\x70\x55\x1B(B", "UTF-32BE", "ISO-2022-JP-KDDI", false);

echo "JIS X 0208 (with MS extensions) and KDDI emoji support OK\n";

testValidString("\x00\xA5", "\x1B\$B!o\x1B(B", "UTF-16BE", "ISO-2022-JP-KDDI", false);
testValidString("\x20\x3E", "\x1B\$B!1\x1B(B", "UTF-16BE", "ISO-2022-JP-KDDI", false);
testValidString("\xFF\x5E", "\x1B\$B!A\x1B(B", "UTF-16BE", "ISO-2022-JP-KDDI", false);

echo "Other mappings from Unicode -> ISO-2022-JP-KDDI OK\n";

testInvalidString("\x1B\$B\x7F\x7E", "%", 'ISO-2022-JP-KDDI', 'UTF-8');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\xE0", "%", "ISO-2022-JP-KDDI", "UTF-8");
// Invalid escapes:
convertInvalidString("\x1B", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B.", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B\$", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B\$.", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B(", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B(.", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B\$(X", "%", "ISO-2022-JP-KDDI", "UTF-8");
convertInvalidString("\x1B\$B\x9F", "%", "ISO-2022-JP-KDDI", "UTF-8"); // 0x9F does not start any 2-byte character
convertInvalidString("\xE0\x00", "U+E000", "UTF-16BE", "ISO-2022-JP-KDDI");

echo "Done!\n";
?>
--EXPECT--
ASCII support OK
JIS X 0201 support OK
JIS X 0208 (with MS extensions) and KDDI emoji support OK
Other mappings from Unicode -> ISO-2022-JP-KDDI OK
Done!
