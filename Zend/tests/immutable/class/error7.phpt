--TEST--
Assignment of nonimmutable objects to properties of immutable objects is not permitted.
--FILE--
<?php
namespace X\Y\Z;
class A {}
immutable class B {
	public $a;
	public function __construct(A $a) {
	    $this->a = $a;
    }
}
new B(new A);
?>
--EXPECTF--

Fatal error: Uncaught Exception: Cannot assign instance of non immutable class X\Y\Z\B to immutable property $a in %s:%d
Stack trace:
#0 %s(%d): X\Y\Z\B->__construct(Object(X\Y\Z\A))
#1 {main}
  thrown in %s on line %d
