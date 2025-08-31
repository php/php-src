--TEST--
Exhaustive test of Shift-JIS DoCoMo, KDDI, SoftBank encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(818); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in Windows-932
 * (The SJIS-Mobile encodings all use MS extensions) */
readConversionTable(__DIR__ . '/data/CP932.txt', $sjisChars, $fromUnicode, true);

/* U+301C (WAVE DASH) converts to SJIS 0x8160 (WAVE DASH) */
$fromUnicode["\x00\x00\x30\x1C"] = "\x81\x60";
/* U+2212 (MINUS SIGN) converts to SJIS 0x817C (FULLWIDTH HYPHEN-MINUS) */
$fromUnicode["\x00\x00\x22\x12"] = "\x81\x7C";
/* U+203E (OVERLINE) converts to SJIS 0x8150 (FULLWIDTH MACRON) */
$fromUnicode["\x00\x00\x20\x3E"] = "\x81\x50";
/* U+2016 (DOUBLE VERTICAL LINE) converts to SJIS 0x8161 (PARALLEL TO) */
$fromUnicode["\x00\x00\x20\x16"] = "\x81\x61";
/* U+00AF (MACRON) converts to SJIS 0x8150 (FULLWIDTH MACRON) */
$fromUnicode["\x00\x00\x00\xAF"] = "\x81\x50";
/* U+00AC (NOT SIGN) converts to SJIS 0x81CA (FULLWIDTH NOT SIGN) */
$fromUnicode["\x00\x00\x00\xAC"] = "\x81\xCA";
/* U+00A5 (YEN SIGN) converts to SJIS 0x818F (FULLWIDTH YEN SIGN) */
$fromUnicode["\x00\x00\x00\xA5"] = "\x81\x8F";
/* U+00A3 (POUND SIGN) converts to SJIS 0x8192 (FULLWIDTH POUND SIGN) */
$fromUnicode["\x00\x00\x00\xA3"] = "\x81\x92";
/* U+00A2 (CENT SIGN) converts to SJIS 0x8191 (FULLWIDTH CENT SIGN) */
$fromUnicode["\x00\x00\x00\xA2"] = "\x81\x91";

/* Aside from the characters in that table, we also support a 'user' area
 * from 0xF040-0xF9FC, which map to Unicode 'private' codepoints 0xE000-E757 */
$codepoint = 0xE000;
for ($i = 0xF0; $i <= 0xF9; $i++) {
  for ($j = 0x40; $j <= 0xFC; $j++) {
    if ($j == 0x7F)
      continue;
    $utf32 = pack('N', $codepoint);
    $cp932 = chr($i) . chr($j);
    $sjisChars[$cp932] = $utf32;
    $fromUnicode[$utf32] = $cp932;
    $codepoint++;
  }
}

$invalidCodepoints = array();
for ($i = 0; $i <= 0xFFFF; $i++) {
  $cp = pack('N', $i);
  if (!isset($fromUnicode[$cp]))
    $invalidCodepoints[$cp] = true;
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
    unset($fromUnicode[$sjisChars[$bytes]]);
  }
}

/* Other "collisions" */
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C, 0xEEF9] as $i) {
  $bytes = pack('n', $i);
  $nonInvertible[$bytes] = $sjisChars[$bytes];
  unset($fromUnicode[$sjisChars[$bytes]]);
}

$nonInvertibleSoftbank = $nonInvertible;
$nonInvertibleDocomo   = $nonInvertible;

/* Now read table of vendor-specific emoji encodings */
$docomo = $sjisChars;
$kddi = $sjisChars;
$softbank = $sjisChars;
$sbEmoji = array();
$fp = fopen(realpath(__DIR__ . '/data/EmojiSources.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;
  $fields = explode(';', rtrim($line));
  if (count($fields) >= 4) {
    if (sscanf($fields[0], "%x %x", $cp1, $cp2) == 2) {
      $utf32 = pack('N', $cp1) . pack('N', $cp2);
    } else {
      $utf32 = pack('N', hexdec($fields[0]));
      unset($invalidCodepoints[$utf32]);
    }

    if ($fields[1])
      $docomo[pack('n', hexdec($fields[1]))] = $utf32;
    if ($fields[2])
      $kddi[pack('n', hexdec($fields[2]))] = $utf32;
    if ($fields[3]) {
      $bytes = pack('n', hexdec($fields[3]));
      $sbEmoji[$bytes] = $utf32;
      unset($nonInvertibleSoftbank[$bytes]);
    }
  }
}

/* Other, vendor-specific emoji which do not appear in EmojiSources.txt
 * Most of these don't exist in Unicode and have been mapped to 'private
 * area' codepoints */
$docomo["\xF9\x4A"] = "\x00\x0F\xEE\x16"; // PIAS PI
$docomo["\xF9\x4B"] = "\x00\x0F\xEE\x17"; // PIAS A
$docomo["\xF9\x4C"] = "\x00\x0F\xEE\x18"; // INVERSE TICKET
$docomo["\xF9\x4D"] = "\x00\x0F\xEE\x19"; // KATAKANA ABBREVIATION FOR TICKET ("chi ke")
$docomo["\xF9\x4E"] = "\x00\x0F\xEE\x1A"; // RESERVE BY PHONE
$docomo["\xF9\x4F"] = "\x00\x0F\xEE\x1B"; // P CODE
$docomo["\xF9\x53"] = "\x00\x0F\xEE\x1C"; // MOVIES 2
$docomo["\xF9\x54"] = "\x00\x0F\xEE\x1D"; // PIAS PI INVERSE
$docomo["\xF9\x58"] = "\x00\x0F\xEE\x1E"; // PIAS PI CIRCLE
$docomo["\xF9\x59"] = "\x00\x0F\xEE\x1F"; // PIAS PI SQUARE
$docomo["\xF9\x5A"] = "\x00\x0F\xEE\x20"; // CHECK
$docomo["\xF9\x5F"] = "\x00\x0F\xEE\x21"; // F
$docomo["\xF9\x60"] = "\x00\x0F\xEE\x22"; // D
$docomo["\xF9\x61"] = "\x00\x0F\xEE\x23"; // S
$docomo["\xF9\x62"] = "\x00\x0F\xEE\x24"; // C
$docomo["\xF9\x63"] = "\x00\x0F\xEE\x25"; // R
$docomo["\xF9\x64"] = "\x00\x00\x25\xEA"; // SQUARE WITH LOWER RIGHT DIAGONAL HALF BLACK
$nonInvertibleDocomo["\xF9\x64"] = "\x00\x00\x25\xEA";
$docomo["\xF9\x65"] = "\x00\x00\x25\xA0"; // BLACK SQUARE
$nonInvertibleDocomo["\xF9\x65"] = "\x00\x00\x25\xA0";
$docomo["\xF9\x66"] = "\x00\x00\x25\xBF"; // DOWNWARD TRIANGLE
$nonInvertibleDocomo["\xF9\x66"] = "\x00\x00\x25\xBF";
/* TODO: test that FEE28 converts to F966, for backwards compatibility */
$docomo["\xF9\x67"] = "\x00\x0F\xEE\x29"; // QUADRUPLE DAGGER
$docomo["\xF9\x68"] = "\x00\x0F\xEE\x2A"; // TRIPLE DAGGER
$docomo["\xF9\x69"] = "\x00\x0F\xEE\x2B"; // DOUBLE DAGGER
$docomo["\xF9\x6A"] = "\x00\x00\x20\x20"; // DAGGER
$nonInvertibleDocomo["\xF9\x6A"] = "\x00\x00\x20\x20";
/* TODO: test that FEE2C converts to F96A, for backwards compatibility */
$docomo["\xF9\x6B"] = "\x00\x0F\xEE\x2D"; // I (meaning "inexpensive")
$docomo["\xF9\x6C"] = "\x00\x0F\xEE\x2E"; // M (meaning "moderate")
$docomo["\xF9\x6D"] = "\x00\x0F\xEE\x2F"; // E (meaning "expensive")
$docomo["\xF9\x6E"] = "\x00\x0F\xEE\x30"; // VE (meaning "very expensive")
$docomo["\xF9\x6F"] = "\x00\x0F\xEE\x31"; // SPHERE
$docomo["\xF9\x70"] = "\x00\x0F\xEE\x32"; // CREDIT CARDS NOT ACCEPTED
$docomo["\xF9\x71"] = "\x00\x0F\xEE\x33"; // CHECKBOX
$docomo["\xF9\x75"] = "\x00\x0F\xEE\x10"; // I-MODE
$docomo["\xF9\x76"] = "\x00\x0F\xEE\x11"; // I-MODE WITH FRAME
$docomo["\xF9\x78"] = "\x00\x0F\xEE\x12"; // PROVIDED BY DOCOMO
$docomo["\xF9\x79"] = "\x00\x0F\xEE\x13"; // DOCOMO POINT
$docomo["\xF9\x84"] = "\x00\x00\x27\xBF"; // FREE DIAL; mapped to DOUBLE CURLY LOOP
unset($invalidCodepoints["\x00\x00\x27\xBF"]);
$docomo["\xF9\x86"] = "\x00\x0F\xE8\x2D"; // MOBILE Q
$docomo["\xF9\xB1"] = "\x00\x0F\xEE\x14"; // I-APPLI
$docomo["\xF9\xB2"] = "\x00\x0F\xEE\x15"; // I-APPLI WITH BORDER

$kddi["\xF7\x94"] = "\x00\x0F\xEE\x40"; // EZ WEB
$kddi["\xF7\xCF"] = "\x00\x0F\xEE\x41"; // EZ PLUS
$kddi["\xF3\x70"] = "\x00\x0F\xEE\x42"; // EZ NAVIGATION
$kddi["\xF4\x78"] = "\x00\x0F\xEE\x43"; // EZ MOVIE
$kddi["\xF4\x86"] = "\x00\x0F\xEE\x44"; // CMAIL
$kddi["\xF4\x8E"] = "\x00\x0F\xEE\x45"; // JAVA (TM)
$kddi["\xF4\x8F"] = "\x00\x0F\xEE\x46"; // BREW
$kddi["\xF4\x90"] = "\x00\x0F\xEE\x47"; // EZ RING MUSIC
$kddi["\xF4\x91"] = "\x00\x0F\xEE\x48"; // EZ NAVI
$kddi["\xF4\x92"] = "\x00\x0F\xEE\x49"; // WIN
$kddi["\xF4\x93"] = "\x00\x0F\xEE\x4A"; // PREMIUM SIGN
$kddi["\xF7\x48"] = "\x00\x0F\xE8\x2D"; // MOBILE Q
$kddi["\xF7\xA3"] = "\x00\x0F\xE8\x3C"; // PDC ("personal digital cellular")
$kddi["\xF7\xD2"] = "\x00\x0F\xEB\x89"; // OPENWAVE

$sbEmoji["\xF7\xB1"] = "\x00\x00\x27\xBF"; // FREE DIAL; mapped to DOUBLE CURLY
$sbEmoji["\xF7\xF4"] = "\x00\x0F\xEE\x77"; // J-PHONE SHOP
$sbEmoji["\xF7\xF5"] = "\x00\x0F\xEE\x78"; // SKY WEB
$sbEmoji["\xF7\xF6"] = "\x00\x0F\xEE\x79"; // SKY WALKER
$sbEmoji["\xF7\xF7"] = "\x00\x0F\xEE\x7A"; // SKY MELODY
$sbEmoji["\xF7\xF8"] = "\x00\x0F\xEE\x7B"; // J-PHONE 1
$sbEmoji["\xF7\xF9"] = "\x00\x0F\xEE\x7C"; // J-PHONE 2
$sbEmoji["\xF7\xFA"] = "\x00\x0F\xEE\x7D"; // J-PHONE 3

/* SoftBank-specific 'JSky1', 'JSky2', 'VODAFONE1', 'VODAFONE2', etc. emoji,
 * which are not supported by Unicode */
for ($i = 0xFBD8; $i <= 0xFBDE; $i++) {
  $bytes = pack('n', $i);
  $sbEmoji[$bytes] = pack('N', 0xFEE70 + $i - 0xFBD8);
  unset($nonInvertibleSoftbank[$bytes]);
}
/* SoftBank-specific emoji for Shibuya department store */
$sbEmoji["\xFB\xAA"] = "\x00\x0F\xE4\xC5";
unset($nonInvertibleSoftbank["\xFB\xAA"]);

$softbank = array_merge($softbank, $sbEmoji);

/* For Softbank, we support an alternative representation for emoji which
 * uses sequences starting with ESC. Apparently this was used in older
 * versions of Softbank's phones.
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
    if (isset($sbEmoji[$sjis])) {
      $bytes = "\x1B\$" . $char . chr($ten);
      $unicode = $softbank[$sjis];
      $nonInvertibleSoftbank[$bytes] = $softbank[$bytes] = $unicode;
    }
  }
}

/* A bare ESC is not valid for Softbank, since it is used for escape sequences
 * which represent emoji */
unset($softbank["\x1B"]);

function testSJISVariant($validChars, $nonInvertible, $encoding) {
  global $fromUnicode, $invalidCodepoints, $escCodeToKu;

  $lenTable = array_fill_keys(range(0xE0, 0xFC), 2) + array_fill_keys(range(0x81, 0x9F), 2);
  findInvalidChars($validChars, $invalidChars, $truncated, $lenTable);

  foreach ($escCodeToKu as $char => $unused) {
    unset($invalidChars["\x1B\$" . $char . "\x0F"]);
    unset($truncated["\x1B\$" . $char]);
  }

  $escapes = [];
  foreach ($nonInvertible as $bytes => $unicode) {
    unset($validChars[$bytes]);
    if (substr($bytes, 0, 1) === "\x1B")
      array_push($escapes, $bytes);
  }
  /* 0xF is used to terminate a run of emoji encoded using ESC sequence
   * We couldn't do this earlier or `findInvalidChars` wouldn't have worked
   * as desired */
  foreach ($escapes as $bytes) {
    $nonInvertible[$bytes . "\x0F"] = $nonInvertible[$bytes];
    unset($nonInvertible[$bytes]);
  }

  testAllValidChars($validChars, $encoding, 'UTF-32BE');
  testAllValidChars($nonInvertible, $encoding, 'UTF-32BE', false);
  echo "$encoding verification and conversion works on all valid characters\n";

  testAllInvalidChars($invalidChars, $validChars, $encoding, 'UTF-32BE', "\x00\x00\x00%");
  testTruncatedChars($truncated, $encoding, 'UTF-32BE', "\x00\x00\x00%");
  echo "$encoding verification and conversion works on all invalid characters\n";

  convertAllInvalidChars($invalidCodepoints, $fromUnicode, 'UTF-32BE', $encoding, '%');
  echo "Unicode -> $encoding conversion works on all invalid codepoints\n";

  // Test "long" illegal character markers
  mb_substitute_character("long");
  convertInvalidString("\x80", "%", $encoding, "UTF-8");
  convertInvalidString("\x81\x20", "%", $encoding, "UTF-8");
  convertInvalidString("\xEA\xA9", "%", $encoding, "UTF-8");
  mb_substitute_character(0x25); // '%'

  // Test Regional Indicator codepoint at end of string
  // The mobile SJIS variants all have special characters to represent certain national
  // flags, but in Unicode these are represented by a sequence of _two_ codepoints
  // So if only one of those two codepoints appears at the end of a string, it can't
  // be converted to SJIS and should be treated as an error
  convertInvalidString("\x00\x01\xF1\xE9", "%", "UTF-32BE", $encoding); // Regional Indicator C

  // Test Regional Indicator codepoint followed by some other codepoint
  convertInvalidString("\x00\x01\xF1\xE9\x00\x00\x00A", "%A", "UTF-32BE", $encoding);
}

testSJISVariant($docomo,   $nonInvertibleDocomo,   'SJIS-Mobile#DOCOMO');
testSJISVariant($kddi,     $nonInvertible,         'SJIS-Mobile#KDDI');
testSJISVariant($softbank, $nonInvertibleSoftbank, 'SJIS-Mobile#SOFTBANK');

// Special Softbank escape sequences can appear at end of string
convertValidString("\x1B\$O", "", "SJIS-Mobile#SOFTBANK", "UTF-8", false);
convertValidString("\x1B\$P", "", "SJIS-Mobile#SOFTBANK", "UTF-8", false);
convertValidString("\x1B\$Q", "", "SJIS-Mobile#SOFTBANK", "UTF-8", false);
// Try invalid escape sequence
convertInvalidString("\x1B\$X", "%", "SJIS-Mobile#SOFTBANK", "UTF-8", false);
// Try truncated escape sequence
convertInvalidString("\x1B\$", "%", "SJIS-Mobile#SOFTBANK", "UTF-8", false);

// Regression test for problem with not allocating enough space in output buffer
// This occurred when the input string was shorter than the output
convertValidString("\xA9\xA9\xA9\xA9", "\xF9\xD6\xF9\xD6\xF9\xD6\xF9\xD6", '8bit', 'SJIS-Mobile#DOCOMO');
convertValidString("\xA9\xA9\xA9\xA9", "\xF7\x74\xF7\x74\xF7\x74\xF7\x74", '8bit', 'SJIS-Mobile#KDDI');
convertValidString("\xA9\xA9\xA9\xA9", "\xF7\xEE\xF7\xEE\xF7\xEE\xF7\xEE", '8bit', 'SJIS-Mobile#SOFTBANK');

// Regression test: Old implementation used to drop digits (0-9) and hash (#) if
// they appeared at end of input string
for ($i = ord('0'); $i <= ord('9'); $i++) {
  convertValidString("abc" . chr($i), "abc" . chr($i), 'UTF-8', 'SJIS-Mobile#DOCOMO');
  convertValidString("abc" . chr($i), "abc" . chr($i), 'UTF-8', 'SJIS-Mobile#KDDI');
  convertValidString("abc" . chr($i), "abc" . chr($i), 'UTF-8', 'SJIS-Mobile#SOFTBANK');
}

// Regression test: Originally, new implementation also did not handle 0-9 and hash
// followed by U+20E3 (keycap modifier) correctly if the 0-9 or hash occurred at
// the very end of one buffer of wchars, and the keycap modifier was at the
// beginning of the following buffer of wchars
for ($i = 0; $i <= 256; $i++) {
  convertValidString(str_repeat("\x00a", $i) . "\x00\x30\x20\xE3", str_repeat('a', $i) . "\xF9\x90", 'UTF-16BE', 'SJIS-Mobile#DOCOMO');
  convertValidString(str_repeat("\x00a", $i) . "\x00\x30\x20\xE3", str_repeat('a', $i) . "\xF7\xC9", 'UTF-16BE', 'SJIS-Mobile#KDDI');
  convertValidString(str_repeat("\x00a", $i) . "\x00\x30\x20\xE3", str_repeat('a', $i) . "\xF7\xC5", 'UTF-16BE', 'SJIS-Mobile#SOFTBANK');
}

// Regression test for 0-9 appearing at end of one buffer and U+203E NOT appearing
// at the beginning of the next
for ($i = 0; $i <= 256; $i++) {
  convertValidString(str_repeat("\x000", $i), str_repeat('0', $i), 'UTF-16BE', 'SJIS-Mobile#DOCOMO');
  convertValidString(str_repeat("\x000", $i), str_repeat('0', $i), 'UTF-16BE', 'SJIS-Mobile#KDDI');
  convertValidString(str_repeat("\x000", $i), str_repeat('0', $i), 'UTF-16BE', 'SJIS-Mobile#SOFTBANK');
}

// Regression test for not making enough space in output buffer when 0-9 appeared
// at the end of one buffer and was re-processed together with the next
// This crazy-looking string was found by a fuzzer
$str = "\x04\xff\x930\x00\xffUTF7~'F\x00A\x00\xffA\x0018030@\x00[\x1b\$EEEEE\x5C\x80(8~\x00F\x00zgb-18030$\x008~\x00F\x00z-gb-18EUC_JP-2004\x00z-g0\x0018030\x00b-18030$\x008~\x00F\x00z-gb-18EUC_JP-2004\x00z-g0\x0018030\x00";
mb_convert_encoding($str, 'SJIS-Mobile#SOFTBANK', 'SJIS-Mobile#SOFTBANK');

?>
--EXPECT--
SJIS-Mobile#DOCOMO verification and conversion works on all valid characters
SJIS-Mobile#DOCOMO verification and conversion works on all invalid characters
Unicode -> SJIS-Mobile#DOCOMO conversion works on all invalid codepoints
SJIS-Mobile#KDDI verification and conversion works on all valid characters
SJIS-Mobile#KDDI verification and conversion works on all invalid characters
Unicode -> SJIS-Mobile#KDDI conversion works on all invalid codepoints
SJIS-Mobile#SOFTBANK verification and conversion works on all valid characters
SJIS-Mobile#SOFTBANK verification and conversion works on all invalid characters
Unicode -> SJIS-Mobile#SOFTBANK conversion works on all invalid codepoints
