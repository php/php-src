--TEST--
Torture test for UTF-{7,8,16,32}
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(232); /* Make results consistent */
mb_substitute_character(0x25); // '%'
include('encoding_tests.inc');

// in UTF-32BE
$validCodepoints = array();
$fp = fopen(realpath(__DIR__ . '/data/UnicodeData.txt'), 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;
  if (sscanf($line, "%x;", $codepoint) == 1)
    if ($codepoint < 0xD800 || $codepoint > 0xDFFF) // surrogates; included in UnicodeData.txt
      $validCodepoints[pack('N', $codepoint)] = true;
}

function testValidCodepoints($encoding) {
  global $validCodepoints;

  $good = array_keys($validCodepoints);
  shuffle($good);

  while (!empty($good)) {
    $string = '';
    $length = min(rand(20,30), count($good));
    while ($length--) {
      $string .= array_pop($good);
    }

    $converted = mb_convert_encoding($string, $encoding, 'UTF-32BE');
    if ($converted === false)
      die("mb_convert_encoding failed to convert UTF-32BE to $encoding." .
          "\nString: " . bin2hex($string));
    testValidString($converted, $string, $encoding, 'UTF-32BE');
  }
}

function testInvalidCodepoints($invalid, $encoding) {
  global $validCodepoints;

  $good = array_keys($validCodepoints);
  shuffle($good);

  foreach ($invalid as $bad => $expected) {
    $good1 = array_pop($good);
    $good2 = array_pop($good);
    $string =  mb_convert_encoding($good1, $encoding, 'UTF-32BE');
    $string .= $bad;
    $string .= mb_convert_encoding($good2, $encoding, 'UTF-32BE');

    testInvalidString($string, $good1 . $expected . $good2, $encoding, 'UTF-32BE');
  }
}

echo "== UTF-8 ==\n";

testValidCodepoints('UTF-8');

testValidString('', '', 'UTF-8', 'UTF-32BE');

$invalid = array(
  // Codepoints outside of valid 0-0x10FFFF range for Unicode
  "\xF4\x90\x80\x80" => "\x00\x00\x00%",                // CP 0x110000
  "\xF7\x80\x80\x80" => str_repeat("\x00\x00\x00%", 4), // CP 0x1C0000
  "\xF7\xBF\xBF\xBF" => str_repeat("\x00\x00\x00%", 4), // CP 0x1FFFFF

  // Reserved range for UTF-16 surrogate pairs
  "\xED\xA0\x80" => str_repeat("\x00\x00\x00%", 2),     // CP 0xD800
  "\xED\xAF\xBF" => str_repeat("\x00\x00\x00%", 2),     // CP 0xDBFF
  "\xED\xBF\xBF" => str_repeat("\x00\x00\x00%", 2),     // CP 0xDFFF

  // Truncated characters
  "\xDF" => "\x00\x00\x00%",         // should have been 2-byte
  "\xEF\xBF" => "\x00\x00\x00%",     // should have been 3-byte
  "\xF0\xBF\xBF" => "\x00\x00\x00%", // should have been 4-byte

  // Multi-byte characters which end too soon and go to ASCII
  "\xDFA" => "\x00\x00\x00%\x00\x00\x00A",
  "\xEF\xBFA" => "\x00\x00\x00%\x00\x00\x00A",
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
  "\xE0\x9F\xBF" => str_repeat("\x00\x00\x00%", 2),    // didn't need 3 bytes
  "\xF0\x8F\xBF\xBF" => str_repeat("\x00\x00\x00%", 3) // didn't need 4 bytes
);

testInvalidCodepoints($invalid, 'UTF-8');

echo "== UTF-16 ==\n";

testValidCodepoints("UTF-16");
testValidCodepoints("UTF-16LE");
testValidCodepoints("UTF-16BE");

testValidString('', '', 'UTF-16', 'UTF-32BE');
testValidString('', '', 'UTF-16LE', 'UTF-32BE');
testValidString('', '', 'UTF-16BE', 'UTF-32BE');

$invalid = array(
  // UTF-16 _cannot_ represent codepoints bigger than 0x10FFFF, so we're not
  // worried about that. But there are plenty of other ways to mess up...

  // Second half of surrogate pair comes first
  "\xDC\x01\xD8\x02" => "\x00\x00\x00%\x00\x00\x00%",

  // First half of surrogate pair not followed by second part
  "\xD8\x01\x00A" => "\x00\x00\x00%\x00\x00\x00A",

  // First half of surrogate pair at end of string
  "\xD8\x01" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-16');
testInvalidCodepoints($invalid, 'UTF-16BE');

// Truncated strings
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16', 'UTF-32BE');
testInvalidString("\x00A\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16', 'UTF-32BE');
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16BE', 'UTF-32BE');
testInvalidString("\x00A\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16BE', 'UTF-32BE');

$invalid = array(
  // Second half of surrogate pair comes first
  "\x01\xDC\x02\xD8" => "\x00\x00\x00%\x00\x00\x00%",

  // First half of surrogate pair not followed by second part
  "\x01\xD8A\x00" => "\x00\x00\x00%\x00\x00\x00A",

  // First half of surrogate pair at end of string
  "\x01\xD8" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-16LE');

// Truncated
testInvalidString("\x00", "\x00\x00\x00%", 'UTF-16LE', 'UTF-32BE');
testInvalidString("A\x00\x01", "\x00\x00\x00A\x00\x00\x00%", 'UTF-16LE', 'UTF-32BE');

// TODO: test handling of UTF-16 BOM

echo "== UTF-32 ==\n";

testValidCodepoints("UTF-32LE");
testValidCodepoints("UTF-32BE");

// Empty string
testValidString('', '', 'UTF-32', 'UTF-32BE');
testValidString('', '', 'UTF-32BE', 'UTF-32');
testValidString('', '', 'UTF-32LE', 'UTF-32BE');

$invalid = array(
  // Codepoints which are too big
  "\x00\x11\x00\x00" => "\x00\x00\x00%",
  "\x80\x00\x00\x00" => "\x00\x00\x00%",
  "\xff\xff\xfe\xff" => "\x00\x00\x00%",

  // Surrogates
  "\x00\x00\xd8\x00" => "\x00\x00\x00%",
  "\x00\x00\xdb\xff" => "\x00\x00\x00%",
  "\x00\x00\xdc\x00" => "\x00\x00\x00%",
  "\x00\x00\xdf\xff" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-32');
testInvalidCodepoints($invalid, 'UTF-32BE');

// Truncated code units
testInvalidString("\x00\x01\x01", "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00\x01",     "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32BE', 'UTF-32');

$invalid = array(
  // Codepoints which are too big
  "\x00\x00\x11\x00" => "\x00\x00\x00%",
  "\x00\x00\x00\x80" => "\x00\x00\x00%",
  "\xff\xfe\xff\xff" => "\x00\x00\x00%",

  // Surrogates
  "\x00\xd8\x00\x00" => "\x00\x00\x00%",
  "\xff\xdb\x00\x00" => "\x00\x00\x00%",
  "\x00\xdc\x00\x00" => "\x00\x00\x00%",
  "\xff\xdf\x00\x00" => "\x00\x00\x00%",
);

testInvalidCodepoints($invalid, 'UTF-32LE');

// Truncated code units
testInvalidString("\x00\x01\x01", "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');
testInvalidString("\x00\x01",     "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');
testInvalidString("\x00",         "\x00\x00\x00%", 'UTF-32LE', 'UTF-32BE');

// TODO: test handling of UTF-32 BOM

echo "== UTF-7 ==\n";

testValidString('', '', 'UTF-7', 'UTF-32BE');

// 'Direct' characters
foreach (range(ord('A'), ord('Z')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (range(ord('a'), ord('z')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (range(ord('0'), ord('9')) as $byte)
  testValidString(chr($byte), "\x00\x00\x00" . chr($byte), 'UTF-7', 'UTF-32BE');
foreach (str_split("'(),-./:?") as $char)
  testValidString($char, "\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE');

// 'Optional direct' characters are Base64-encoded in mbstring's implementation

// Whitespace
foreach (str_split(" \t\r\n\x00") as $char)
  testValidString($char, "\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE');

// Encoding + as +-
testValidString('+-', "\x00\x00\x00+", 'UTF-7', 'UTF-32BE', false);

// UTF-16 + Base64 encoding
function encode($str, $encoding) {
  // Base64 encoding for UTF-7 doesn't use '=' for padding
  return str_replace('=', '', base64_encode(mb_convert_encoding($str, 'UTF-16BE', $encoding)));
}

for ($i = 0; $i < 256; $i++) {
  $reversible = true;
  if ($i >= ord('A') && $i <= ord('Z'))
    $reversible = false;
  if ($i >= ord('a') && $i <= ord('z'))
    $reversible = false;
  if ($i >= ord('0') && $i <= ord('9'))
    $reversible = false;
  if (strpos("'(),-./:?\x00 \t\r\n", chr($i)) !== false)
    $reversible = false;

  testValidString('+' . encode("\x00" . chr($i), 'UTF-16BE') . '-', "\x00\x00\x00" . chr($i), 'UTF-7', 'UTF-32BE', $reversible);
}

testValidString('+' . encode("\x12\x34", 'UTF-16BE') . '-', "\x00\x00\x12\x34", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x12\x34\x56\x78", 'UTF-16BE') . '-', "\x00\x00\x12\x34\x00\x00\x56\x78", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x12\x34\x56\x78\x00\x40", 'UTF-16BE') . '-', "\x00\x00\x12\x34\x00\x00\x56\x78\x00\x00\x00\x40", 'UTF-7', 'UTF-32BE');

// Surrogate pair
testValidString('+' . encode("\x00\x01\x04\x00", 'UTF-32BE') . '-', "\x00\x01\x04\x00", 'UTF-7', 'UTF-32BE');
testValidString('+' . encode("\x00\x00\x00A\x00\x01\x04\x00\x00\x00\x00B", 'UTF-32BE') . '-', "\x00\x00\x00A\x00\x01\x04\x00\x00\x00\x00B", 'UTF-7', 'UTF-32BE', false);
testValidString('+' . encode("\x00\x01\x04\x00\x00\x01\x04\x00", 'UTF-32BE') . '-', "\x00\x01\x04\x00\x00\x01\x04\x00", 'UTF-7', 'UTF-32BE');

// Unterminated + section
// (This is not considered illegal)
testValidString('+' . encode('ABC', 'ASCII'), "\x00A\x00B\x00C", 'UTF-7', 'UTF-16BE', false);

// + sections immediately after each other
// (This isn't illegal either)
testValidString('+' . encode('AB', 'ASCII') . '-+' . encode('CD', 'ASCII') . '-', "\x00A\x00B\x00C\x00D", 'UTF-7', 'UTF-16BE', false);

// + sections not immediately after each other
// (Just trying to be exhaustive here)
testValidString('+' . encode('AB', 'ASCII') . '-!+' . encode('CD', 'ASCII') . '-', "\x00A\x00B\x00!\x00C\x00D", 'UTF-7', 'UTF-16BE', false);

// + section terminated by a non-Base64 ASCII character which is NOT -
for ($i = 0; $i < 128; $i++)  {
  if ($i >= ord('A') && $i <= ord('Z'))
    continue;
  if ($i >= ord('a') && $i <= ord('z'))
    continue;
  if ($i >= ord('0') && $i <= ord('9'))
    continue;
  if ($i == ord('+') || $i == ord('/') || $i == ord('-') || $i == ord('\\') || $i == ord('~'))
    continue;
  $char = chr($i);
  testValidString('+' . encode("\x12\x34", 'UTF-16BE') . $char, "\x00\x00\x12\x34\x00\x00\x00" . $char, 'UTF-7', 'UTF-32BE', false);
}

// Now let's see how UTF-7 can go BAD...

function rawEncode($str) {
  return str_replace('=', '', base64_encode($str));
}

// First, messed up UTF16 in + section
// Second half of surrogate pair coming first
testInvalidString('+' . rawEncode("\xDC\x01\xD8\x02") . '-', "\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// First half of surrogate pair not followed by second half
testInvalidString('+' . rawEncode("\xD8\x01\x00A") . '-', "\x00\x00\x00%\x00\x00\x00A", 'UTF-7', 'UTF-32BE');
testInvalidString('+' . rawEncode("\xD8\x01\xD9\x02") . '-', "\x00\x00\x00%\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// First half of surrogate pair appearing at end of string
testInvalidString('+' . rawEncode("\xD8\x01") . '-', "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// Truncated string
testInvalidString('+' . rawEncode("\x01") . '-', "\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

// And then, messed up Base64 encoding

// Bad padding on + section (not zeroes)
$encoded = encode("\x12\x34", 'UTF-16BE'); // 3 Base64 bytes, 2 bits of padding...
$corrupted = substr($encoded, 0, 2) . chr(ord($encoded[2]) + 1);
testInvalidString('+' . $corrupted . '-', "\x00\x00\x12\x34\x00\x00\x00%", 'UTF-7', 'UTF-32BE');

echo "Done!\n";

?>
--EXPECT--
== UTF-8 ==
== UTF-16 ==
== UTF-32 ==
== UTF-7 ==
Done!
