--TEST--
Bug #79947: Memory leak on invalid offset type in compound assignment
--FILE--
<?php
$array = [];
$key = [];
$array[$key] += [$key];
var_dump($array);
?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d
array(0) {
}
