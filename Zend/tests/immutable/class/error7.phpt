--TEST--
Assignment of nonimmutable objects to properties of immutable objects are not permitted.
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

Fatal error: Uncaught Error: Immutable property X\Y\Z\B::$a must hold instance of immutable class, got mutable X\Y\Z\A instance in %simmutable_modifier%sclass%serror7.php:8
Stack trace:
#0 %s: X\Y\Z\B->__construct(Object(X\Y\Z\A))
#1 {main}
  thrown in %simmutable%sclass%serror7.php on line 11
