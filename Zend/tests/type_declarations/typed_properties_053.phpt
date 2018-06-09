--TEST--
Default values of callable properties
--FILE--
<?php
class A {
	public callable $a1;
	public ?callable $a2 = null;
}
$obj = new A;
var_dump($obj);
?>
--EXPECT--
object(A)#1 (1) {
  ["a1"]=>
  uninitialized(callable)
  ["a2"]=>
  NULL
}
