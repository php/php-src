--TEST--
zend multibyte (2)
--EXTENSIONS--
mbstring
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
zend.multibyte=On
zend.script_encoding=UTF-8
internal_encoding=CP932
--FILE--
<?php
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "836583588367"
