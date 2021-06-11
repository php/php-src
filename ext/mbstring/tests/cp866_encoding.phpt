--TEST--
Exhaustive test of verification and conversion of CP866 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP866.txt', 'CP866');
?>
--EXPECT--
Tested CP866 -> UTF-16BE
Tested UTF-16BE -> CP866
