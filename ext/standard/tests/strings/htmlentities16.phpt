--TEST--
htmlentities() test 16 (mbstring / cp1251)
--INI--
output_handler=
mbstring.internal_encoding=cp1251
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
?>
--FILE--
<?php
$str = "\x88\xa9\xf0\xee\xf1\xea\xee\xf8\xed\xfb\xe9";
var_dump(bin2hex($str), bin2hex(htmlentities($str, ENT_QUOTES, '')));
var_dump(htmlentities($str, ENT_QUOTES | ENT_HTML5, ''));
?>
===DONE===
--EXPECT--
string(22) "88a9f0eef1eaeef8edfbe9"
string(42) "266575726f3b26636f70793bf0eef1eaeef8edfbe9"
string(58) "&euro;&copy;&rcy;&ocy;&scy;&kcy;&ocy;&shcy;&ncy;&ycy;&jcy;"
===DONE===
