--TEST--
Check for problems with case sensitivity in compositions
--FILE--
<?php
error_reporting(E_ALL);

trait A {
	public function M1() {}
	public function M2() {}
}

trait B {
	public function M1() {}
	public function M2() {}
}

class MyClass {
	use A;
	use B;
}
?>
--EXPECTF--
Warning: Trait method M1 has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d

Warning: Trait method M2 has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d