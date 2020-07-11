--TEST--
Bug #78069 (Out-of-bounds read in iconv.c:_php_iconv_mime_decode() due to integer overflow)
--SKIPIF--
<?php
if (!extension_loaded('iconv')) die('skip ext/iconv required');
?>
--FILE--
<?php
$hdr = iconv_mime_decode_headers(file_get_contents(__DIR__ . "/bug78069.data"),2);
var_dump(count($hdr));
?>
DONE
--EXPECT--
int(1)
DONE