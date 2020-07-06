--TEST--
Typed class constants (type mismatch; runtime)
--FILE--
<?php
class A {
	public const string A = A;
}

define('A', 1);

echo A::A;
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign int to class constant A::A of type string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
