--TEST--
zend multibyte (14)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
zend.script_encoding=UTF-8
internal_encoding=UTF-8
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
string(24) "c3a4c3abc3bcc3a1c3a9c3ba"
