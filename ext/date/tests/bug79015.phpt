--TEST--
Bug #79015 (undefined-behavior in php_date.c)
--FILE--
<?php
var_dump(unserialize('O:12:"DateInterval":1:1s:1:"f";i:9999999999990;'));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 47 of 47 bytes in %s on line %d
bool(false)
