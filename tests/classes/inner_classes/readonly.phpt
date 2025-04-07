--TEST--
readonly should work
--FILE--
<?php

class Outer {
    public readonly class Inner { public function __construct(public int $t) {} }
}

var_dump($foo = new Outer\Inner(1));
$foo->t = 42;
var_dump($foo);
?>
--EXPECTF--
object(Outer\Inner)#1 (1) {
  ["t"]=>
  int(1)
}

Fatal error: Uncaught Error: Cannot modify readonly property Outer\Inner::$t in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
