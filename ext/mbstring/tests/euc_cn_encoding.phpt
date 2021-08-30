--TEST--
Exhaustive test of verification and conversion of EUC-CN text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/EUC-CN.txt', 'EUC-CN');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "EUC-CN", "UTF-8");
convertInvalidString("\xA1\x50", "%", "EUC-CN", "UTF-8");
convertInvalidString("\xF7\xFF", "%", "EUC-CN", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested EUC-CN -> UTF-16BE
Tested UTF-16BE -> EUC-CN
Done!
