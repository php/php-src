--TEST--
SplFileObject::fpassthru function - basic functionality test
--FILE--
<?php
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
$obj->fpassthru();
?>
--EXPECT--
first,second,third
1,2,3
4,5,6
7,8,9
0,0,0
