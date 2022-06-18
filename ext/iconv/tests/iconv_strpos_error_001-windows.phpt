--TEST--
iconv_strpos() - test against PHP's defaults internal encoding. Windows systems only.
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) !== "WIN" ) {
    die("skip test is for windows systems only");
}
?>
--FILE--
<?php
$large_enconding = str_repeat("X", 256);
ini_set("default_charset", $large_enconding);
var_dump(iconv_strpos('Hello, world', 'world', -2));
?>
--EXPECTF--
Warning: iconv_strpos(): Wrong encoding, conversion from %s
bool(false)
