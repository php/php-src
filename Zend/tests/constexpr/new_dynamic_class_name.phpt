--TEST--
Dynamic class name in new is not supported
--FILE--
<?php

class Foo {}
const FOO = 'Foo';
static $x = new (FOO);

var_dump($x);

?>
--EXPECT--
object(Foo)#1 (0) {
}
