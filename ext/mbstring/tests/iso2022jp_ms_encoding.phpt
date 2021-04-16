--TEST--
Exhaustive test of ISO-2022-JP-MS text encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(444); /* Make results consistent */
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
    /* ISO-2022-JP-MS only uses the first two ranges of MS vendor extensions */
    if ($bytes >= 0xFA00)
      continue;
    $cp932Chars[pack('n', shiftJISDecode($bytes))] = pack('N', $codepoint);
  }
}

/* Windows-932 has many cases where two different kuten codes map to the same
 * Unicode codepoints */
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C, 0xEEF9] as $i) {
  $bytes = pack('n', shiftJISDecode($i));
  $nonInvertible[$bytes] = $cp932Chars[$bytes];
}

/* Add User Defined codes (which use ESC $ ( ? escape sequence)) */
$udcChars = array();
for ($cp = 0xE000; $cp < (0xE000 + (20 * 94)); $cp++) {
  $i = $cp - 0xE000;
  $bytes = ((($i / 94) + 0x7F - 0x5E) << 8) + (($i % 94) + 0x21);
  $udcChars[pack('n', $bytes)] = pack('N', $cp);
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
    if (strpos($from, "\x1B\$B") !== false || strpos($from, "\x1B(I") !== false || strpos($from, "\x1B\$@") !== false || strpos($from, "\x1B\$(B") !== false || strpos($from, "\x1B\$(@") !== false || strpos($from, "\x1B\$(?") !== false)
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
  testValid(chr($i),            "\x00\x00\x00" . chr($i), 'ISO-2022-JP-MS');
  testValid("\x1B(B" . chr($i), "\x00\x00\x00" . chr($i), 'ISO-2022-JP-MS', false);
  testValid("\x1B(J" . chr($i), "\x00\x00\x00" . chr($i), 'ISO-2022-JP-MS', false);
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF) // We convert single bytes from 0xA1-0xDF as JIS X 0201 kana
    continue;
  testInvalid(chr($i),            "\x00\x00\x00%", 'ISO-2022-JP-MS');
  testInvalid("\x1B(B" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-MS');
  testInvalid("\x1B(J" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-MS');
}

echo "ASCII support OK\n";

/* All valid JIS X 0201 characters
 * Those with a 1 in the high bit are JIS X 0201 kana */
foreach ($jisx0201Chars as $jisx0201 => $utf32BE) {
  if (ord($jisx0201) >= 128) {
    $kana = chr(ord($jisx0201) - 128);
    testValid("\x1B(I" . $kana, $utf32BE, 'ISO-2022-JP-MS', false);
    testValid($jisx0201, $utf32BE, 'ISO-2022-JP-MS', false);
  }
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF)
    continue;
  testInvalid("\x1B(I" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-MS');
  testInvalid("\x1B(J" . chr($i), "\x00\x00\x00%", 'ISO-2022-JP-MS');
}

echo "JIS X 0201 support OK\n";

function testAllValidCharsWithPrefix($validChars, $prefix, $bothWays) {
  $good = array_keys($validChars);
  shuffle($good);
  while (!empty($good)) {
    $length = min(rand(5,10), count($good));
    $from = $to = '';
    while ($length--) {
      $goodChar = array_pop($good);
      $from .= $goodChar;
      $to .= $validChars[$goodChar];
    }
    testValid($prefix . $from, $to, 'ISO-2022-JP-MS', $bothWays);
  }
}

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

testAllValidCharsWithPrefix($cp932Chars, "\x1B\$B", true);
testAllValidCharsWithPrefix($nonInvertible, "\x1B\$B", false);

foreach (array_keys($invalidChars) as $invalid) {
  $firstByte = ord($invalid[0]);
  if (($firstByte > 0x80 && $firstByte < 0xA0) || $firstByte >= 0xE0) {
    /* The first byte of this 2-byte character will be rejected and result in % being sent
     * to the output. Then the second byte will do something else. It is easier to write the
     * test if we only check with the 1st byte. */
    testInvalidString("\x1B\$B" . $invalid[0], "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');
  } else {
    testInvalidString("\x1B\$B" . $invalid, "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');
  }
}
foreach (array_keys($truncatedChars) as $truncated)
  testInvalidString("\x1B\$B" . $truncated, "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');

echo "JIS X 0208 (with MS extensions) support OK\n";

$validChars = $udcChars;
/* We allow ASCII/JIS X 0201 characters to appear even in JIS X 0208 mode */
for ($i = 0; $i <= 0x7F; $i++)
  $validChars[chr($i)] = chr($i);
for ($i = 0xA1; $i <= 0xDF; $i++)
  $validChars[chr($i)] = $jisx0201Chars[chr($i)];
$lenTable = array_fill_keys(range(0xE0, 0xFC), 2) + array_fill_keys(range(0x81, 0x9F), 2);
findInvalidChars($validChars, $invalidChars, $truncatedChars, $lenTable);

testAllValidCharsWithPrefix($udcChars, "\x1B\$(?", true);

foreach (array_keys($invalidChars) as $invalid) {
  $firstByte = ord($invalid[0]);
  if (($firstByte > 0x80 && $firstByte < 0xA0) || $firstByte >= 0xE0) {
    testInvalidString("\x1B\$(?" . $invalid[0], "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');
  } else {
    testInvalidString("\x1B\$(?" . $invalid, "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');
  }
}
foreach (array_keys($truncatedChars) as $truncated)
  testInvalidString("\x1B\$(?" . $truncated, "\x00\x00\x00%", 'ISO-2022-JP-MS', 'UTF-32BE');

echo "UDC support OK\n";

?>
--EXPECT--
ASCII support OK
JIS X 0201 support OK
JIS X 0208 (with MS extensions) support OK
UDC support OK
