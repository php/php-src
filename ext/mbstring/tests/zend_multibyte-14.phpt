--TEST--
zend multibyte (14)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--INI--
mbstring.script_encoding=UTF-8
mbstring.internal_encoding=UTF-8
--FILE--
<?php
var_dump(substr(file_get_contents(__FILE__), __COMPILER_HALT_OFFSET__));
var_dump(bin2hex("äëüáéú"));
__halt_compiler();test
test
--EXPECT--
string(10) "test
test
"
string(24) "c3a4c3abc3bcc3a1c3a9c3ba"
