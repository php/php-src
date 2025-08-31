--TEST--
Exhaustive test of verification and conversion of KOI8-U text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
testEncodingFromUTF16ConversionTable(__DIR__ . '/data/KOI8-U.txt', 'KOI8-U');
/* Try replacement character which cannot be encoded in KOI8-U; ? will be used instead */
mb_substitute_character(0x1234);
convertInvalidString("\x23\x45", '?', 'UTF-16BE', 'KOI8-U');
?>
--EXPECT--
Tested KOI8-U -> UTF-16BE
Tested UTF-16BE -> KOI8-U
