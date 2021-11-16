--TEST--
Exhaustive test of verification and conversion of EUC-KR text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/EUC-KR.txt', 'EUC-KR');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "EUC-KR", "UTF-8");
convertInvalidString("\xA7\xF0", "%", "EUC-KR", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested EUC-KR -> UTF-16BE
Tested UTF-16BE -> EUC-KR
Done!
