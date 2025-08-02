--TEST--
Bug #33999 (object remains object when cast to int)
--INI--
error_reporting=4095
--FILE--
<?php
class Foo {
  public $bar = "bat";
}

$foo = new Foo;
var_dump($foo);

$bar = (int)$foo;
var_dump($bar);

$baz = (float)$foo;
var_dump($baz);
?>
--EXPECTF--
object(Foo)#1 (1) {
  ["bar"]=>
  string(3) "bat"
}

Warning: Object of class Foo could not be converted to int in %s on line %d
int(1)

Warning: Object of class Foo could not be converted to float in %s on line %d
float(1)
