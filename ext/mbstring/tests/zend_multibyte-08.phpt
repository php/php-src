--TEST--
zend multibyte (8)
--SKIPIF--
--INI--
zend.multibyte=On
zend.script_encoding=ISO-8859-1
mbstring.internal_encoding=UTF-8
--FILE--
<?php
declare(encoding="CP932");
var_dump(bin2hex("ƒeƒXƒg"));
?>
--EXPECT--
string(18) "e38386e382b9e38388"
