--TEST--
Pass short ctor parameters
--FILE--
<?php

abstract class Family(protected $string);

class Child1($string) extends Family ($this->internal_string) {
	private $internal_string = "internal";
}

var_dump(new Child1("test"));
?>
--EXPECTF--
object(Child1)#1 (2) {
  ["string":protected]=>
  string(8) "internal"
  ["internal_string":"Child1":private]=>
  string(8) "internal"
}