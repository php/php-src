--TEST--
Pass short ctor parameters
--FILE--
<?php

abstract class Family(protected $string);

class Child1($string) extends Family ($string) {}

var_dump(new Child1("test"));
?>
--EXPECTF--
object(Child1)#1 (1) {
  ["string":protected]=>
  string(4) "test"
}