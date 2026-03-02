--TEST--
Temporary test of mbstring's Base64 'encoding'
--EXTENSIONS--
mbstring
--FILE--
<?php

/* Using mbstring to convert strings to and from Base64 has already been deprecated
 * So this test should be removed when the Base64 'encoding' is */

function testConversion($raw, $base64) {
  $converted = mb_convert_encoding($raw, 'Base64', '8bit');
  if ($converted !== $base64)
    die('Expected ' . bin2hex($raw) . ' to convert to "' . $base64 . '"; actually got "' . $converted . '"');
  $converted = mb_convert_encoding($base64, '8bit', 'Base64');
  if ($converted !== $raw)
    die('Expected "' . $base64 . '" to convert to ' . bin2hex($raw) . '; actually got ' . bin2hex($converted));
}

testConversion('', '');
testConversion('a', 'YQ==');
testConversion('ab', 'YWI=');
testConversion("\x01\x02\x03", 'AQID');
testConversion("\xFF\xFE\x11\x22", '//4RIg==');
testConversion("\x00", 'AA==');
testConversion("\x00\x00", 'AAA=');
testConversion("\x00\x00\x00", 'AAAA');

testConversion(str_repeat("ABCDEFGHIJ", 20), "QUJDREVGR0hJSkFCQ0RFRkdISUpBQkNERUZHSElKQUJDREVGR0hJSkFCQ0RFRkdISUpBQkNERUZH\r\nSElKQUJDREVGR0hJSkFCQ0RFRkdISUpBQkNERUZHSElKQUJDREVGR0hJSkFCQ0RFRkdISUpBQkNE\r\nRUZHSElKQUJDREVGR0hJSkFCQ0RFRkdISUpBQkNERUZHSElKQUJDREVGR0hJSkFCQ0RFRkdISUpB\r\nQkNERUZHSElKQUJDREVGR0hJSkFCQ0RFRkdISUo=");

echo "Done!\n";
?>
--EXPECTF--
Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s
Done!
