--TEST--
Exhaustive test of verification and conversion of CP949 (UHC) text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP949.txt', 'UHC');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "UHC", "UTF-8");
convertInvalidString("\xA7\xF0", "%", "UHC", "UTF-8");
?>
--EXPECT--
Tested UHC -> UTF-16BE
Tested UTF-16BE -> UHC
