--TEST--
Incompatible context call (non-internal function)
--INI--
error_reporting=E_ALL
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
Deprecated: Non-static method A::foo() should not be called statically, assuming $this from incompatible context in %s on line %d
string(1) "B"
