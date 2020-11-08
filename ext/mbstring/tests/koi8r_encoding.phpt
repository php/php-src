--TEST--
Exhaustive test of verification and conversion of KOI8-R text
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/KOI8-R.txt', 'KOI8-R');
?>
--EXPECT--
Tested KOI8-R -> UTF-16BE
Tested UTF-16BE -> KOI8-R
