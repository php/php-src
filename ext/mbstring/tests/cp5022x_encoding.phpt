--TEST--
Exhaustive test of CP50220, CP50221, and CP50222 encodings
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

function shiftJISDecode($bytes) {
  /* Convert CP932's default Shift-JIS representation to kuten code
   *
   * Shift-JIS is fun! The first byte only represents the top 7 bits of
   * the ku number, because 94 first bytes were not available. There are
   * two different ranges of 94 which the second byte can fall in, and
   * we get the low bit of the ku number by seeing which one it is. */
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

/* Read in table of all characters in CP932 charset */
$cp932Chars = array(); /* CP932 -> UTF-16BE */
$nonInvertible = array();
$fromUnicode = array();
$fp = fopen(__DIR__ . '/data/CP932.txt', 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
    if ($bytes < 256)
      continue;


    if (isset($fromUnicode[$codepoint])) {
      $nonInvertible[pack('n', shiftJISDecode($bytes))] = pack('n', $codepoint);
    } else {
      $cp932Chars[pack('n', shiftJISDecode($bytes))] = pack('n', $codepoint);
      $fromUnicode[$codepoint] = $bytes;
    }
  }
}

/* Aside from the characters in that table, we also support a 'user' area,
 * which maps to Unicode 'private' codepoints 0xE000-E757 */
$codepoint = 0xE000;
for ($i = 0xF0; $i <= 0xF9; $i++) {
  for ($j = 0x40; $j <= 0xFC; $j++) {
    if ($j == 0x7F)
      continue;
    $cp932Chars[pack('n', shiftJISDecode(($i << 8) + $j))] = pack('n', $codepoint);
    $codepoint++;
  }
}

/* Read in table of all characters in JISX-0201 charset */
$jisx0201Chars = array(); /* JISX0201 -> UTF-16BE */
$fp = fopen(__DIR__ . '/data/JISX0201.txt', 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $byte, $codepoint) == 2)
    $jisx0201Chars[chr($byte)] = pack('n', $codepoint);
}

/* Read in table of all characters in JISX-0212 charset */
$jisx0212Chars = array();
$fp = fopen(__DIR__ . '/data/JISX0212.txt', 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
    $jisx0212Chars[pack('n', $bytes)] = pack('n', $codepoint);
  }
}

/* Our conversions between CP5022x (when CP932 charset is selected) and Unicode
 * differ in a number of places from the table provided by the Unicode Consortium */
$cp932Chars["\x21\x41"] = "\x30\x1C"; /* WAVE DASH instead of FULLWIDTH TILDE */
$cp932Chars["\x21\x42"] = "\x20\x16"; /* DOUBLE VERTICAL LINE instead of PARALLEL TO */
$cp932Chars["\x21\x5D"] = "\x22\x12"; /* MINUS SIGN instead of FULLWIDTH HYPHEN-MINUS */
$cp932Chars["\x21\x71"] = "\x00\xA2"; /* CENT SIGN instead of FULLWIDTH CENT SIGN */
$cp932Chars["\x21\x72"] = "\x00\xA3"; /* POUND SIGN instead of FULLWIDTH POUND SIGN */
$cp932Chars["\x22\x4C"] = "\x00\xAC"; /* NOT SIGN instead of FULLWIDTH NOT SIGN */

function testValid($from, $to, $encoding, $bothWays = true) {
  identifyValidString($from, $encoding);
  convertValidString($from, $to, $encoding, 'UTF-16BE', false);

  if ($bothWays) {
    /* An 0xF at the beginning is redundant; it switches to ASCII mode, but
     * ASCII mode is default */
    if ($from[0] == "\x0F")
      $from = substr($from, 1, strlen($from) - 1);
    /* ESC ( B at the beginning is redundant, since ASCII mode is the default */
    if (substr($from, 0, 3) == "\x1B(B")
      $from = substr($from, 3, strlen($from) - 3);
    /* If the string switches to a different charset, it should switch back to
     * ASCII at the end */
    if (strpos($from, "\x1B\$B") !== false || strpos($from, "\x1B(J") !== false || strpos($from, "\x1B(I") !== false)
      $from .= "\x1B(B";
    if ($encoding == 'CP50222' && $from[0] == "\x0E")
      $from .= "\x0F";

    convertValidString($to, $from, 'UTF-16BE', $encoding, false);
  }
}

function testInvalid($from, $to, $encoding) {
  testInvalidString($from, $to, $encoding, 'UTF-16BE');
}

for ($i = 0; $i < 0x80; $i++) {
  if ($i == 0xE || $i == 0xF || $i == 0x1B)
    continue;
  testValid(chr($i),            "\x00" . chr($i), 'CP50220');
  testValid(chr($i),            "\x00" . chr($i), 'CP50221');
  testValid(chr($i),            "\x00" . chr($i), 'CP50222');
  testValid("\x1B(B" . chr($i), "\x00" . chr($i), 'CP50220');
  testValid("\x1B(B" . chr($i), "\x00" . chr($i), 'CP50221');
  testValid("\x1B(B" . chr($i), "\x00" . chr($i), 'CP50222');
  testValid("\x0F" . chr($i),   "\x00" . chr($i), 'CP50222', false); /* 0xF is 'Shift Out' code */
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF) // We convert single bytes from 0xA1-0xDF as JIS X 0201 kana
    continue;
  testInvalid(chr($i),            "\x00%", 'CP50220');
  testInvalid(chr($i),            "\x00%", 'CP50221');
  testInvalid(chr($i),            "\x00%", 'CP50222');
  testInvalid("\x1B(B" . chr($i), "\x00%", 'CP50220');
  testInvalid("\x1B(B" . chr($i), "\x00%", 'CP50221');
  testInvalid("\x1B(B" . chr($i), "\x00%", 'CP50222');
  testInvalid("\x0F" . chr($i),   "\x00%", 'CP50220');
  testInvalid("\x0F" . chr($i),   "\x00%", 'CP50221');
  testInvalid("\x0F" . chr($i),   "\x00%", 'CP50222');
}

// Switch back to ASCII after a multibyte character
convertValidString("\x30\x00\x00a\x00b\x00c", "\x1B\$B\x21\x21\x1B(Babc", 'UTF-16BE', 'CP50221', false);
convertValidString("\x30\x00\x00a\x00b\x00c", "\x1B\$B\x21\x21\x1B(Babc", 'UTF-16BE', 'CP50222', false);

echo "ASCII support OK\n";

/* All valid JIS X 0201 characters
 * Those with a 1 in the high bit are JIS X 0201 kana */
foreach ($jisx0201Chars as $jisx0201 => $utf16BE) {
  if (ord($jisx0201) >= 128) { /* Kana */
    $kana = chr(ord($jisx0201) - 128);
    testValid("\x1B(I" . $kana, $utf16BE, 'CP50221');
    testValid("\x1B(J\x0E" . $kana, $utf16BE, 'CP50222', false); /* 0xE is 'Shift In' code */
    testValid("\x0E" . $kana, $utf16BE, 'CP50222', false);
    testValid($jisx0201, $utf16BE, 'CP50220', false);
    testValid($jisx0201, $utf16BE, 'CP50221', false);
    testValid($jisx0201, $utf16BE, 'CP50222', false);
    convertValidString($utf16BE, "\x0E" . chr(ord($jisx0201) - 0x80) . "\x0F", 'UTF-16BE', 'CP50222', false);
  } else { /* Latin */
    testValid("\x1B(J" . $jisx0201, $utf16BE, 'CP50220', $utf16BE > "\x00\x80");
    testValid("\x1B(J" . $jisx0201, $utf16BE, 'CP50221', $utf16BE > "\x00\x80");
    testValid("\x1B(J" . $jisx0201, $utf16BE, 'CP50222', $utf16BE > "\x00\x80");
  }
}

for ($i = 0x80; $i < 256; $i++) {
  if ($i >= 0xA1 && $i <= 0xDF)
    continue;
  testInvalid("\x1B(I" . chr($i), "\x00%", 'CP50220');
  testInvalid("\x1B(I" . chr($i), "\x00%", 'CP50221');
  testInvalid("\x1B(I" . chr($i), "\x00%", 'CP50222');
  testInvalid("\x1B(J" . chr($i), "\x00%", 'CP50220');
  testInvalid("\x1B(J" . chr($i), "\x00%", 'CP50221');
  testInvalid("\x1B(J" . chr($i), "\x00%", 'CP50222');
}

/* Go from JIS X 0201 to ASCII or JIS X 0208 */
convertValidString("\xFF\x61\x00A", "\x0E\x21\x0FA", 'UTF-16BE', 'CP50222', false);
convertValidString("\xFF\x61\x22\x25", "\x0E\x21\x0F\x1B\$B\x21\x42\x1B(B", 'UTF-16BE', 'CP50222', false);
convertValidString("\xFF\x61\x20\x3E", "\x0E\x21\x0F\x1B(J\x7E\x1B(B", 'UTF-16BE', 'CP50222');

echo "JIS X 0201 support OK\n";

/* All valid CP932 characters */
foreach ($cp932Chars as $cp932 => $utf16BE) {
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50220');
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50221');
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50222');
}
foreach ($nonInvertible as $cp932 => $utf16BE) {
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50220', false);
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50221', false);
  testValid("\x1B\$B" . $cp932, $utf16BE, 'CP50222', false);
}

/* There are some conversions we support from Unicode -> CP5022x, but not in the opposite direction */
foreach (['CP50220', 'CP50221', 'CP50222'] as $encoding) {
  convertValidString("\x22\x25", "\x1B\$B\x21\x42\x1B(B", 'UTF-16BE', $encoding, false);
  convertValidString("\xFF\x0D", "\x1B\$B\x21\x5D\x1B(B", 'UTF-16BE', $encoding, false);
  convertValidString("\xFF\xE0", "\x1B\$B\x21\x71\x1B(B", 'UTF-16BE', $encoding, false);
  convertValidString("\xFF\xE1", "\x1B\$B\x21\x72\x1B(B", 'UTF-16BE', $encoding, false);
  convertValidString("\xFF\xE2", "\x1B\$B\x22\x4C\x1B(B", 'UTF-16BE', $encoding, false);
}

/* All invalid 2-byte CP932 characters */
for ($i = 0x21; $i <= 0x97; $i++) {
  for ($j = 0; $j < 256; $j++) {
    $testString = chr($i) . chr($j);
    if (!isset($cp932Chars[$testString]) && !isset($nonInvertible[$testString])) {
      testInvalid("\x1B\$B" . $testString, "\x00%", 'CP50220');
      testInvalid("\x1B\$B" . $testString, "\x00%", 'CP50221');
      testInvalid("\x1B\$B" . $testString, "\x00%", 'CP50222');
    }
  }
}

/* Try truncated 2-byte characters */
for ($i = 0x21; $i <= 0x97; $i++) {
  testInvalid("\x1B\$B" . chr($i), "\x00%", 'CP50220');
  testInvalid("\x1B\$B" . chr($i), "\x00%", 'CP50221');
  testInvalid("\x1B\$B" . chr($i), "\x00%", 'CP50222');
}

/* Test alternative escape sequence to select CP932 */
testValid("\x1B\$(B\x21\x21", "\x30\x00", 'CP50220', false);

echo "CP932 support OK\n";

foreach ($jisx0212Chars as $jisx0212 => $utf16BE) {
  testValid("\x1B\$(D" . $jisx0212, $utf16BE, 'CP50220', false);
  testValid("\x1B\$(D" . $jisx0212, $utf16BE, 'CP50221', false);
  testValid("\x1B\$(D" . $jisx0212, $utf16BE, 'CP50222', false);
}

for ($i = 0x21; $i <= 0x97; $i++) {
  for ($j = 0; $j < 256; $j++) {
    $testString = chr($i) . chr($j);
    if (!isset($jisx0212Chars[$testString])) {
      testInvalid("\x1B\$(D" . $testString, "\x00%", 'CP50220');
      testInvalid("\x1B\$(D" . $testString, "\x00%", 'CP50221');
      testInvalid("\x1B\$(D" . $testString, "\x00%", 'CP50222');
    }
  }
}

for ($i = 0x21; $i <= 0x97; $i++) {
  testInvalid("\x1B\$(D" . chr($i), "\x00%", 'CP50220');
  testInvalid("\x1B\$(D" . chr($i), "\x00%", 'CP50221');
  testInvalid("\x1B\$(D" . chr($i), "\x00%", 'CP50222');
}

echo "JIS X 0212 support OK\n";

/* Unicode codepoint for halfwidth katakana -> kuten code for ordinary katakana */
$fullwidthKatakana = array(
  0xFF61 => 0x2123, /* Ideographic full stop */
  0xFF62 => 0x2156, /* Left corner bracket */
  0xFF63 => 0x2157, /* Right corner bracket */
  0xFF64 => 0x2122, /* Ideographic comma */
  0xFF65 => 0x2126, /* Katakana middle dot */
  0xFF66 => 0x2572, /* Wo */
  0xFF67 => 0x2521, /* Small A */
  0xFF68 => 0x2523, /* Small I */
  0xFF69 => 0x2525, /* Small U */
  0xFF6A => 0x2527, /* Small E */
  0xFF6B => 0x2529, /* Small O */
  0xFF6C => 0x2563, /* Small Ya */
  0xFF6D => 0x2565, /* Small Yu */
  0xFF6E => 0x2567, /* Small Yo */
  0xFF6F => 0x2543, /* Small Tsu */
  0xFF70 => 0x213C, /* Prolonged Sound Marker */
  0xFF71 => 0x2522, /* A */
  0xFF72 => 0x2524, /* I */
  0xFF73 => 0x2526, /* U */
  0xFF74 => 0x2528, /* E */
  0xFF75 => 0x252A, /* O */
  0xFF76 => 0x252B, /* Ka */
  0xFF77 => 0x252D, /* Ki */
  0xFF78 => 0x252F, /* Ku */
  0xFF79 => 0x2531, /* Ke */
  0xFF7A => 0x2533, /* Ko */
  0xFF7B => 0x2535, /* Sa */
  0xFF7C => 0x2537, /* Shi */
  0xFF7D => 0x2539, /* Su */
  0xFF7E => 0x253B, /* Se */
  0xFF7F => 0x253D, /* So */
  0xFF80 => 0x253F, /* Ta */
  0xFF81 => 0x2541, /* Chi */
  0xFF82 => 0x2544, /* Tsu */
  0xFF83 => 0x2546, /* Te */
  0xFF84 => 0x2548, /* To */
  0xFF85 => 0x254A, /* Na */
  0xFF86 => 0x254B, /* Ni */
  0xFF87 => 0x254C, /* Nu */
  0xFF88 => 0x254D, /* Ne */
  0xFF89 => 0x254E, /* No */
  0xFF8A => 0x254F, /* Ha */
  0xFF8B => 0x2552, /* Hi */
  0xFF8C => 0x2555, /* Fu */
  0xFF8D => 0x2558, /* He */
  0xFF8E => 0x255B, /* Ho */
  0xFF8F => 0x255E, /* Ma */
  0xFF90 => 0x255F, /* Mi */
  0xFF91 => 0x2560, /* Mu */
  0xFF92 => 0x2561, /* Me */
  0xFF93 => 0x2562, /* Mo */
  0xFF94 => 0x2564, /* Ya */
  0xFF95 => 0x2566, /* Yu */
  0xFF96 => 0x2568, /* Yo */
  0xFF97 => 0x2569, /* Ra */
  0xFF98 => 0x256A, /* Ri */
  0xFF99 => 0x256B, /* Ru */
  0xFF9A => 0x256C, /* Re */
  0xFF9B => 0x256D, /* Ro */
  0xFF9C => 0x256F, /* Wa */
  0xFF9D => 0x2573, /* N */
  0xFF9E => 0x212B, /* Voice Mark */
  0xFF9F => 0x212C  /* Semi-voice Mark */
);
foreach ($fullwidthKatakana as $cp => $kuten) {
  convertValidString(pack('n', $cp), "\x1B\$B" . pack('n', $kuten) . "\x1B(B", 'UTF-16BE', 'CP50220', false);
}

echo "Folding of fullwidth katakana for CP50220 OK\n";

testInvalidString("\xD8\x00", '%', 'UTF-16BE', 'CP50220');
testInvalidString("\xD8\x00", '%', 'UTF-16BE', 'CP50221');
testInvalidString("\xD8\x00", '%', 'UTF-16BE', 'CP50222');

echo "Invalid Unicode is flagged when converting to CP5022x\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "CP50220", "UTF-8");
convertInvalidString("\x80", "%", "CP50221", "UTF-8");
convertInvalidString("\x80", "%", "CP50222", "UTF-8");
convertInvalidString("\x1B\$B1", "%", "CP50220", "UTF-8");
convertInvalidString("\x1B\$B1", "%", "CP50221", "UTF-8");
convertInvalidString("\x1B\$B1", "%", "CP50222", "UTF-8");

echo "Long error markers OK\n";

foreach (['CP50220', 'CP50221', 'CP50222'] as $encoding) {
  testInvalidString("\x1B", "%", $encoding, "UTF-8");
  testInvalidString("\x1BX", "%X", $encoding, "UTF-8");
  testInvalidString("\x1B(", "%", $encoding, "UTF-8");
  testInvalidString("\x1B(X", "%(X", $encoding, "UTF-8");
  testInvalidString("\x1B\$", "%", $encoding, "UTF-8");
  testInvalidString("\x1B\$(", "%", $encoding, "UTF-8");
  testInvalidString("\x1B\$X", "%\$X", $encoding, "UTF-8");
  testInvalidString("\x1B\$(X", "%\$(X", $encoding, "UTF-8");
}

echo "Invalid escape sequences OK\n";

// Regression tests
if (mb_convert_encoding("\x1BC\xF5", 'UTF-16BE', 'CP50221') !== "\x00%\x00C\x00%")
  die("Bad");

// Previously, the CP50220 implementation would eat trailing null bytes
$converted = mb_convert_encoding("ab\x00", 'UTF-16BE', 'CP50220');
if ($converted !== "\x00a\x00b\x00\x00")
  die("Bad handling of trailing null byte (got " . bin2hex($converted) . ")");

// Previously, the CP50220 implementation would reorder error markers with
// subsequent characters
mb_substitute_character(0x3F);
$converted = mb_convert_encoding("\xff\xff\x00&", 'CP50220', 'UTF-16BE');
if ($converted !== '?&')
  die("Bad handling of erroneous codepoint followed by good one (got " . bin2hex($converted) . ")");

// In CP50220, two codepoints can be collapsed into a single kuten code in some cases
// This should work even on a boundary between separately processed buffers
$shouldCollapse = "\xFF\x76\xFF\x9E";
$expected = "\x1B\$B%,\x1B(B";
for ($i = 0; $i < 256; $i++) {
  convertValidString(str_repeat("\x00a", $i) . $shouldCollapse, str_repeat('a', $i) . $expected, 'UTF-16BE', 'CP50220', false);
}

?>
--EXPECT--
ASCII support OK
JIS X 0201 support OK
CP932 support OK
JIS X 0212 support OK
Folding of fullwidth katakana for CP50220 OK
Invalid Unicode is flagged when converting to CP5022x
Long error markers OK
Invalid escape sequences OK
