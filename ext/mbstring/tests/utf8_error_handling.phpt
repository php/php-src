--TEST--
Confirm error handling for UTF-8 complies with WHATWG spec
--EXTENSIONS--
mbstring
--FILE--
<?php
/* The WHATWG specifies not just how web browsers should handle _valid_
 * UTF-8 text, but how they should handle _invalid_ UTF-8 text (such
 * as how many error markers each invalid byte sequence should decode
 * to).
 * That specification is followed by the JavaScript Encoding API.
 *
 * The API documentation for mb_convert_encoding does not specify how
 * many error markers we will emit for each possible invalid byte
 * sequence, so we might as well comply with the WHATWG specification.
 *
 * Thanks to Martin Auswöger for pointing this out... and another big
 * thanks for providing test cases!
 *
 * Ref: https://encoding.spec.whatwg.org/#utf-8-decoder
 */
mb_substitute_character(0x25);

$testCases = [
  ["\x80", "%"],
  ["\xFF", "%"],
  ["\xC2\x7F", "%\x7F"],
  ["\xC2\x80", "\xC2\x80"],
  ["\xDF\xBF", "\xDF\xBF"],
  ["\xDF\xC0", "%%"],
  ["\xE0\xA0\x7F", "%\x7F"],
  ["\xE0\xA0\x80", "\xE0\xA0\x80"],
  ["\xEF\xBF\xBF", "\xEF\xBF\xBF"],
  ["\xEF\xBF\xC0", "%%"],
  ["\xF0\x90\x80\x7F", "%\x7F"],
  ["\xF0\x90\x80\x80", "\xF0\x90\x80\x80"],
  ["\xF4\x8F\xBF\xBF", "\xF4\x8F\xBF\xBF"],
  ["\xF4\x8F\xBF\xC0", "%%"],
  ["\xFA\x80\x80\x80\x80", "%%%%%"],
  ["\xFB\xBF\xBF\xBF\xBF", "%%%%%"],
  ["\xFD\x80\x80\x80\x80\x80", "%%%%%%"],
  ["\xFD\xBF\xBF\xBF\xBF\xBF", "%%%%%%"]
];

foreach ($testCases as $testCase) {
  $result = mb_convert_encoding($testCase[0], 'UTF-8', 'UTF-8');
  if ($result !== $testCase[1]) {
    die("Expected UTF-8 string " . bin2hex($testCase[0]) . " to convert to UTF-8 string " . bin2hex($testCase[1]) . "; got " . bin2hex($result));
  }
}

echo "All done!\n";

?>
--EXPECT--
All done!
