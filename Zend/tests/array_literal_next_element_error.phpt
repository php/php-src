--TEST--
Next free element may overflow in array literals
--FILE--
<?php

$i = PHP_INT_MAX;
$array = [$i => 42, new stdClass];
var_dump($array);

const FOO = [PHP_INT_MAX => 42, "foo"];
var_dump(FOO);

?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(1) {
  [%d]=>
  int(42)
}

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(1) {
  [%d]=>
  int(42)
}
