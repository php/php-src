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
Deprecated: Non-static method A::foo() should not be called statically in %s on line %d

Notice: Undefined variable: this in %s on line %d
string(1) "A"
