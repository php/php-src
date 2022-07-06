--TEST--
empty() on array elements
--FILE--
<?php
$a=array('0','empty'=>'0');
var_dump(empty($a['empty']));
var_dump(empty($a[0]));
$b='0';
var_dump(empty($b));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
