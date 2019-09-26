--TEST--
Bug #52237 (Crash when passing the reference of the property of a non-object)
--FILE--
<?php
$data = [PHP_INT_MAX => 'test'];
preg_match('//', '', $data[]);
var_dump(count($data));
?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
int(1)
