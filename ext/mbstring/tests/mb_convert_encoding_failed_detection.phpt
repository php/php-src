--TEST--
mb_convert_encoding() when encoding detection fails
--FILE--
<?php

var_dump(mb_convert_encoding("\xff", "ASCII", ["UTF-8", "UTF-16"]));

$str = "\xff";
var_dump(mb_convert_variables("ASCII", ["UTF-8", "UTF-16"], $str));
var_dump(bin2hex($str));

?>
--EXPECTF--
Warning: mb_convert_encoding(): Unable to detect character encoding in %s on line %d
bool(false)

Warning: mb_convert_variables(): Unable to detect encoding in %s on line %d
bool(false)
string(2) "ff"
