--TEST--
zend multibyte (4)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
zend.script_encoding=CP932
internal_encoding=UTF-8
--FILE--
<?php
var_dump(bin2hex("ƒeƒXƒg"));
?>
--EXPECT--
string(18) "e38386e382b9e38388"
