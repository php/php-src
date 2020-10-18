--TEST--
Exhaustive test of verification and conversion of CP1254 text
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP1254.txt', 'CP1254');
?>
--EXPECT--
Tested CP1254 -> UTF-16BE
Tested UTF-16BE -> CP1254
