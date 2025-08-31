--TEST--
Exhaustive test of UTF-8 text encoding (DoCoMo, KDDI, SoftBank variants)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(855); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

$badUTF8 = array(
  // Codepoints outside of valid 0-0x10FFFF range for Unicode
  "\xF4\x90\x80\x80" => str_repeat("\x00\x00\x00%", 4), // CP 0x110000
  "\xF7\x80\x80\x80" => str_repeat("\x00\x00\x00%", 4), // CP 0x1C0000
  "\xF7\xBF\xBF\xBF" => str_repeat("\x00\x00\x00%", 4), // CP 0x1FFFFF

  // Reserved range for UTF-16 surrogate pairs
  "\xED\xA0\x80" => str_repeat("\x00\x00\x00%", 3),     // CP 0xD800
  "\xED\xAF\xBF" => str_repeat("\x00\x00\x00%", 3),     // CP 0xDBFF
  "\xED\xBF\xBF" => str_repeat("\x00\x00\x00%", 3),     // CP 0xDFFF

  // Truncated characters
  "\xDF" => "\x00\x00\x00%",         // should have been 2-byte
  "\xEF\xBF" => "\x00\x00\x00%",     // should have been 3-byte
  "\xF0\xBF\xBF" => "\x00\x00\x00%", // should have been 4-byte
  "\xF1\x96" => "\x00\x00\x00%",
  "\xF1\x96\x80" => "\x00\x00\x00%",
  "\xF2\x94" => "\x00\x00\x00%",
  "\xF2\x94\x80" => "\x00\x00\x00%",
  "\xF3\x94" => "\x00\x00\x00%",
  "\xF3\x94\x80" => "\x00\x00\x00%",
  "\xE0\x9F" => "\x00\x00\x00%\x00\x00\x00%",
  "\xED\xA6" => "\x00\x00\x00%\x00\x00\x00%",

  // Multi-byte characters which end too soon and go to ASCII
  "\xDFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xEF\xBFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xF0\xBFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xF0\xBF\xBFA" => "\x00\x00\x00%\x00\x00\x00A",

  // Multi-byte characters which end too soon and go to another MB char
  "\xDF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",
  "\xEF\xBF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",
  "\xF0\xBF\xBF\xDF\xBF" => "\x00\x00\x00%\x00\x00\x07\xFF",

  // Continuation bytes which appear outside of a MB char
  "\x80" => "\x00\x00\x00%",
  "A\x80" => "\x00\x00\x00A\x00\x00\x00%",
  "\xDF\xBF\x80" => "\x00\x00\x07\xFF\x00\x00\x00%",

  // Overlong code units
  // (Using more bytes than needed to encode a character)
  "\xC1\xBF" => str_repeat("\x00\x00\x00%", 2),        // didn't need 2 bytes
  "\xE0\x9F\xBF" => str_repeat("\x00\x00\x00%", 3),    // didn't need 3 bytes
  "\xF0\x8F\xBF\xBF" => str_repeat("\x00\x00\x00%", 4) // didn't need 4 bytes
);

function intToString($value) {
  if ($value <= 0xFF)
      return chr($value);
  else if ($value <= 0xFFFF)
      return pack('n', $value);
  else if ($value <= 0xFFFFFF)
      return chr($value >> 16) . pack('n', $value & 0xFFFF);
  else
      return pack('N', $value);
}

function readUTF8ConversionTable($path, &$from, &$to, &$invalid) {
    $from = array();
    $to   = array();
    $invalid = array();

    $fp = fopen($path, 'r+');
    while ($line = fgets($fp, 256)) {
        if (sscanf($line, "0x%x\t0x%x", $codepoint, $char) == 2) {
            $codepoint = pack('N', $codepoint);
            $char = intToString($char);
            $from[$char] = $codepoint;
            $to[$codepoint] = $char;
        } else if (sscanf($line, "0x%x\tBAD", $codepoint) == 1) {
          $codepoint = pack('N', $codepoint);
          $invalid[$codepoint] = true;
        }
    }
}

function testUTF8Variant($encoding, $filename) {
    readUTF8ConversionTable(__DIR__ . $filename, $toUnicode, $fromUnicode, $invalidCodepoints);

    // Test some plain, vanilla codepoints (to/from mobile encoding)
    testValidString("\x00\x00", "\x00", "UTF-16BE", $encoding);

    for ($i = 0; $i < 1000; $i++) {
      $cp = pack('N', rand(1, 0x10FFFF));
      if (isset($fromUnicode[$cp]))
        continue;
      if (mb_convert_encoding($cp, $encoding, 'UTF-32BE') !== mb_convert_encoding($cp, 'UTF-8', 'UTF-32BE'))
        die("Expected U+" . bin2hex($cp) . " to be the same in UTF-8 and " . $encoding);
    }

    if ($encoding === 'UTF-8-Mobile#DOCOMO') {
      // In Docomo Shift-JIS, we have mappings for U+FEE16 up to U+FEE25 and
      // then U+FEE29-U+FEE2B, U+FEE2D-U+FEE33
      // These correspond to sequential Docomo SJIS codes, but in the middle there is
      // one emoji which converts to U+25EA (SQUARE WITH LOWER RIGHT DIAGONAL HALF BLACK)

      // However, when converting Unicode to Docomo vendor-specific encodings, we still
      // accept U+FEE26 and convert it to the same SQUARE WITH LOWER RIGHT DIAGONAL HALF BLACK emoji
      // So our mapping for U+FEE26 is not reversible
      // Encoded as UTF-8, that's EE9B80
      unset($toUnicode["\xEE\x9B\x80"]);
      // Similar for U+FEE27, U+FEE28, U+FEE2C
      unset($toUnicode["\xEE\x9B\x81"]);
      unset($toUnicode["\xEE\x9B\x82"]);
      unset($toUnicode["\xEE\x9B\x86"]);
    }

    // Test all characters which are different in mobile encoding (from standard UTF-8)
    foreach ($toUnicode as $char => $cp)
      testValidString($char, $cp, $encoding, 'UCS-4BE', false);
    foreach ($fromUnicode as $cp => $char)
      testValidString($cp, $char, 'UCS-4BE', $encoding, false);
    foreach ($invalidCodepoints as $cp => $_)
      convertInvalidString($cp, '%', 'UCS-4BE', $encoding);

    // Try malformed UTF-8 sequences
    global $badUTF8;
    foreach ($badUTF8 as $invalidText => $expectedResult)
      testInvalidString($invalidText, $expectedResult, $encoding, 'UCS-4BE');

    echo "$encoding OK\n";
}

testUTF8Variant('UTF-8-Mobile#DOCOMO', '/data/UTF-8-DOCOMO.txt');
testUTF8Variant('UTF-8-Mobile#KDDI-A', '/data/UTF-8-KDDI-A.txt');
testUTF8Variant('UTF-8-Mobile#KDDI-B', '/data/UTF-8-KDDI-B.txt');
testUTF8Variant('UTF-8-Mobile#SOFTBANK', '/data/UTF-8-SOFTBANK.txt');

?>
--EXPECT--
UTF-8-Mobile#DOCOMO OK
UTF-8-Mobile#KDDI-A OK
UTF-8-Mobile#KDDI-B OK
UTF-8-Mobile#SOFTBANK OK
