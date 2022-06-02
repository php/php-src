--TEST--
Passing an object of a class with no __set_state method to var_export emits a deprecation
--FILE--
<?php

class Foo {
    public static function __set_state(array $values) {}
}

class Bar {}

var_export(new Foo());
var_export(new Bar());
var_export([new Bar()]);

?>
--EXPECTF--
\Foo::__set_state(array(
))
Deprecated: Passing object of class (Bar) with no __set_state method is deprecated in %s on line %d
\Bar::__set_state(array(
))
Deprecated: Passing object of class (Bar) with no __set_state method is deprecated in %s on line %d
array (
  0 => 
  \Bar::__set_state(array(
  )),
)
