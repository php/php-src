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
Fatal error: Non-static method A::foo() cannot be called statically, assuming $this from incompatible context in %s on line %d
