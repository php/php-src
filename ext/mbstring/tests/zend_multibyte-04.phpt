--TEST--
zend multibyte (4)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--INI--
mbstring.script_encoding=CP932
mbstring.internal_encoding=UTF-8
--FILE--
<?php
var_dump(bin2hex("ƒeƒXƒg"));
?>
--EXPECT--
string(18) "e38386e382b9e38388"
