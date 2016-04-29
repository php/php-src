--TEST--
Incompatible context call (non-internal function)
--FILE--
<?php

class A {
    function foo() { var_dump(get_class($this)); }
}
class B {
    function bar() { A::foo(); }
}
$b = new B;
$b->bar();

?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method A::foo() cannot be called statically in %s:%d
Stack trace:
#0 %s(%d): B->bar()
#1 {main}
  thrown in %s on line %d
