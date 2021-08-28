--TEST--
Exhaustive test of verification and conversion of CP1254 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP1254.txt', 'CP1254');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x81", "BAD+81", "CP1254", "UTF-8");
convertInvalidString("\x9E", "BAD+9E", "CP1254", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested CP1254 -> UTF-16BE
Tested UTF-16BE -> CP1254
Done!
