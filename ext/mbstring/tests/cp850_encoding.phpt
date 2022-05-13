--TEST--
Exhaustive test of verification and conversion of CP850 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/CP850.txt', 'CP850');
/* Try replacement character which cannot be encoded in CP850; ? will be used instead */
mb_substitute_character(0x1234);
convertInvalidString("\x23\x45", '?', 'UTF-16BE', 'CP850');
?>
--EXPECT--
Tested CP850 -> UTF-16BE
Tested UTF-16BE -> CP850
