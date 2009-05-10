--TEST--
zend multibyte (13)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--INI--
mbstring.script_encoding=UTF-8
mbstring.internal_encoding=ISO-8859-1
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
string(12) "e4ebfce1e9fa"
