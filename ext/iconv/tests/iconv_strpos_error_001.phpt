--TEST--
iconv_strpos() - test against PHP's defaults internal encoding.
--EXTENSIONS--
iconv
--FILE--
<?php
$large_enconding = str_repeat("X", 256);
ini_set("default_charset", $large_enconding);
var_dump(iconv_strpos('Hello, world', 'world', -2));
?>
--EXPECTF--
Warning: ini_set(): Unknown encoding %s

Warning: ini_set(): INI setting contains invalid encoding %s

Warning: iconv_strpos(): Wrong encoding, conversion from %s
bool(false)
