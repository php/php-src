--TEST--
Pass short ctor parameters
--FILE--
<?php

abstract class Family(protected $string);

class Child1($string) extends Family (self::VALUE) {
	const VALUE = 123;
}

var_dump(new Child1("test"));
?>
--EXPECTF--
object(Child1)#1 (1) {
  ["string":protected]=>
  int(123)
}