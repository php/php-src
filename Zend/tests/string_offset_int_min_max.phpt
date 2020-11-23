--TEST--
Accessing PHP_INT_MAX and PHP_INT_MIN as string offsets
--FILE--
<?php

$str = "";
var_dump($str[PHP_INT_MAX]);
var_dump($str[PHP_INT_MIN]);

?>
--EXPECTF--
Warning: Uninitialized string offset %d in %s on line %d
string(0) ""

Warning: Uninitialized string offset -%d in %s on line %d
string(0) ""
