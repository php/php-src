--TEST--
Test mt_getrandmax() - basic function test mt_getrandmax()
--FILE--
<?php
var_dump(mt_getrandmax());
?>
--EXPECTF--
int(%d)
