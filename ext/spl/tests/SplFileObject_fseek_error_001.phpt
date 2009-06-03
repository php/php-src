--TEST--
SplFileObject::fseek function - parameters test
--FILE--
<?php
$obj = New SplFileObject(__FILE__);
$obj->fseek(1,2,3);
$obj->fseek();
?>
--EXPECTF--
Warning: SplFileObject::fseek() expects at most 2 parameters, 3 given %s

Warning: SplFileObject::fseek() expects at least 1 parameter, 0 given %s
