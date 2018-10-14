--TEST--
SplFileObject::fpassthru function - parameters test
--FILE--
<?php
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
$obj->fwrite();
$obj->fwrite('6,6,6',25,null);
?>
--EXPECTF--
Warning: SplFileObject::fwrite() expects at least 1 parameter, 0 given in %s

Warning: SplFileObject::fwrite() expects at most 2 parameters, 3 given in %s
