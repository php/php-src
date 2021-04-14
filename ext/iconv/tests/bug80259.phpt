--TEST--
Bug #80259 (Bad handling of RFC 2047 in case of folding white space)
--SKIPIF--
<?php
if (!extension_loaded("iconv")) die("skip iconv extension not available");
?>
--FILE--
<?php
var_dump(iconv_mime_decode("=?UTF-8?Q?foo?= bar"));
var_dump(iconv_mime_decode("=?UTF-8?Q?foo?= =?UTF-8?Q?bar?="));
?>
--EXPECT--
string(7) "foo bar"
string(6) "foobar"
