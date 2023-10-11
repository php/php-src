--TEST--
zend multibyte (13)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
zend.script_encoding=UTF-8
internal_encoding=ISO-8859-1
--FILE--
<?php
var_dump(substr(file_get_contents(__FILE__), __COMPILER_HALT_OFFSET__));
var_dump(bin2hex("äëüáéú"));
__halt_compiler();test
test
?>
--EXPECT--
string(10) "test
test
"
string(12) "e4ebfce1e9fa"
