--TEST--
Test typed properties work fine with simple inheritance
--FILE--
<?php

class A {
	public int $a = 1;
}
class B extends A {}

$o = new B;
var_dump($o->a);
$o->a = "a";

?>
--EXPECTF--
int(1)

Fatal error: Uncaught TypeError: Typed property A::$a must be int, string used in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
