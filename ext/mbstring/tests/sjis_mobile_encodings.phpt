--TEST--
Exhaustive test of Shift-JIS DoCoMo, KDDI, SoftBank encoding verification and conversion
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(818); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in Windows-932
 * (The SJIS-Mobile encodings all use MS extensions) */
$sjisChars = array(); /* Windows-932 string -> UTF-32BE string */
$fp = fopen(realpath(__DIR__ . '/data/CP932.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
    if ($bytes < 256)
      $sjisChars[chr($bytes)] = pack('N', $codepoint);
    else
      $sjisChars[pack('n', $bytes)] = pack('N', $codepoint);
  }
}

/* Windows-932 has many cases where two different kuten codes map to the same
 * Unicode codepoints
 *
 * Everything from 0xED00-0xEEFF falls in this unfortunate category
 * (Other sequences in 0xFA00-0xFC4B map to the same codepoints.)
 * Our implementation of CP932 prefers the F's, but for SJIS-Mobile,
 * we prefer the E's */
$nonInvertible = array();
for ($i = 0xFA00; $i <= 0xFC4B; $i++) {
  $bytes = pack('n', $i);
  if (isset($sjisChars[$bytes])) {
    $nonInvertible[$bytes] = $sjisChars[$bytes];
  }
}

/* Other "collisions" */
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C, 0xEEF9] as $i) {
  $bytes = pack('n', $i);
  $nonInvertible[$bytes] = $sjisChars[$bytes];
}

$nonInvertibleSoftbank = $nonInvertible;

/* Now read table of vendor-specific emoji encodings */
$docomo = $sjisChars;
$kddi = $sjisChars;
$softbank = $sjisChars;
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

    if ($fields[1])
      $docomo[pack('n', hexdec($fields[1]))] = $utf32;
    if ($fields[2])
      $kddi[pack('n', hexdec($fields[2]))] = $utf32;
    if ($fields[3]) {
      $bytes = pack('n', hexdec($fields[3]));
      $softbank[$bytes] = $utf32;
      unset($nonInvertibleSoftbank[$bytes]);
    }
  }
}

/* For Softbank, we support an alternative representation for emoji which
 * uses sequences starting with ESC. Apparently this was used in older versions
 * of Softbank's phones.
 * ESC could be followed by 6 different ASCII characters, each of which
 * represented a different ku code */
$escCodeToKu = array('G' => 0x91, 'E' => 0x8D, 'F' => 0x8E, 'O' => 0x92, 'P' => 0x95, 'Q' => 0x96);
$escCodeMaxTen = array('G' => 0x7A, 'E' => 0x7A, 'F' => 0x7A, 'O' => 0x6D, 'P' => 0x6C, 'Q' => 0x5E);

function shiftJISEncode($ku, $ten) {
  $ku -= 0x21;
  $ten -= 0x21;
  $hiBits = $ku >> 1;
  $loBit  = $ku % 2;
  if ($hiBits < 31) {
    $sjis = chr($hiBits + 0x81);
  } else {
    $sjis = chr($hiBits - 31 + 0xE0);
  }
  if ($loBit == 0) {
    if ($ten < 63)
      return $sjis . chr($ten + 0x40);
    else
      return $sjis . chr($ten - 63 + 0x80);
  } else {
    return $sjis . chr($ten + 0x9F);
  }
}

foreach ($escCodeToKu as $char => $ku) {
  for ($ten = 0x21; $ten <= $escCodeMaxTen[$char]; $ten++) {
    $sjis = shiftJISEncode($ku, $ten);
    if ($sjis == "\xFB\x40" || $sjis == "\xFB\x9F" || $sjis == "\xFB\xA0" || $sjis == "\xFB\xAA" || $sjis == "\xFB\xD8" || $sjis == "\xFB\xD9" || $sjis == "\xFB\xDA" || $sjis == "\xFB\xDB" || $sjis == "\xFB\xDC")
      continue;
    if (isset($softbank[$sjis])) {
      /* 0xF is used to terminate a run of emoji encoded using ESC sequence */
      $bytes = "\x1B\$" . $char . chr($ten) . "\xF";
      $softbank[$bytes] = $softbank[$sjis];
      $nonInvertibleSoftbank[$bytes] = $softbank[$sjis];
    }
  }
}

/* A bare ESC is not valid for Softbank, since it is used for escape sequences
 * which represent emoji */
unset($softbank["\x1B"]);

/* SoftBank-specific 'JSky1', 'JSky2', 'VODAFONE1', 'VODAFONE2', etc. emoji,
 * which are not supported by Unicode */
for ($i = 0xFBD8; $i <= 0xFBDE; $i++) {
  $bytes = pack('n', $i);
  $softbank[$bytes] = pack('N', 0xFEE70 + $i - 0xFBD8);
  unset($nonInvertibleSoftbank[$bytes]);
}
/* SoftBank-specific emoji for Shibuya department store */
$softbank["\xFB\xAA"] = "\x00\x0F\xE4\xC5";
unset($nonInvertibleSoftbank["\xFB\xAA"]);

function testSJISVariant($validChars, $nonInvertible, $encoding) {
  $lenTable = map(range(0xE0, 0xFC), 2, map(range(0x81, 0x9F), 2));
  findInvalidChars($validChars, $invalidChars, $truncated, $lenTable);

  foreach ($nonInvertible as $bytes => $unicode)
    unset($validChars[$bytes]);

  testAllValidChars($validChars, $encoding, 'UTF-32BE');
  testAllValidChars($nonInvertible, $encoding, 'UTF-32BE', false);
  echo "$encoding verification and conversion works on all valid characters\n";

  testAllInvalidChars($invalidChars, $validChars, $encoding, 'UTF-32BE', "\x00\x00\x00%");
  testTruncatedChars($truncated, $encoding, 'UTF-32BE', "\x00\x00\x00%");
  echo "$encoding verification and conversion works on all invalid characters\n";
}

testSJISVariant($docomo,   $nonInvertible,         'SJIS-Mobile#DOCOMO');
testSJISVariant($kddi,     $nonInvertible,         'SJIS-Mobile#KDDI');
testSJISVariant($softbank, $nonInvertibleSoftbank, 'SJIS-Mobile#SOFTBANK');

?>
--EXPECT--
SJIS-Mobile#DOCOMO verification and conversion works on all valid characters
SJIS-Mobile#DOCOMO verification and conversion works on all invalid characters
SJIS-Mobile#KDDI verification and conversion works on all valid characters
SJIS-Mobile#KDDI verification and conversion works on all invalid characters
SJIS-Mobile#SOFTBANK verification and conversion works on all valid characters
SJIS-Mobile#SOFTBANK verification and conversion works on all invalid characters
