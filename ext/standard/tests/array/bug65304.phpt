--TEST--
Bug #65304 (Use of max int in array_sum)
--FILE--
<?php
var_dump(array_sum(array(PHP_INT_MAX, 1)));
var_dump(PHP_INT_MAX + 1);
?>
--EXPECTF--	
float(%s)
float(%s)
