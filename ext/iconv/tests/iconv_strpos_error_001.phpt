--TEST--
iconv_strpos() - test against PHP's default internal encoding
--EXTENSIONS--
iconv
--FILE--
<?php
$large_enconding = str_repeat("X", 256);
ini_set("default_charset", $large_enconding);
var_dump(iconv_strpos('Hello, world', 'world', -2));
?>
--EXPECTF--
%AWarning: iconv_strpos(): Wrong encoding, conversion from %s
bool(false)
