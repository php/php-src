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
?>
--EXPECT--
Tested EUC-KR -> UTF-16BE
Tested UTF-16BE -> EUC-KR
