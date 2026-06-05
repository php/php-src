--TEST--
Class name with incorrect case fails in instanceof operator
--FILE--
<?php
class Foo {}

$o = new Foo();

var_dump($o instanceof Foo);   // correct case
var_dump($o instanceof FOO);   // wrong case - false
var_dump($o instanceof foo);   // wrong case - false
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
