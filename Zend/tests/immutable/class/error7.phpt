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

Fatal error: Uncaught Error: Cannot assign instance of non immutable class X\Y\Z\B to immutable property $a in thrown in %serror7.php:7
Stack trace:
#0 thrown in %serror7.php(10): X\Y\Z\B->__construct(Object(X\Y\Z\A))
#1 {main}
  thrown in %serror7.php on line 7
