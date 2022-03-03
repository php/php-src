--TEST--
Exhaustive test of verification and conversion of CP1251 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP1251.txt', 'CP1251');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x98", "%", "CP1251", "UTF-8");
convertInvalidString("\x12\x34", "U+1234", "UTF-16BE", "CP1251");

echo "Done!\n";
?>
--EXPECT--
Tested CP1251 -> UTF-16BE
Tested UTF-16BE -> CP1251
Done!
