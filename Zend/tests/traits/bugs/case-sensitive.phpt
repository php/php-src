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
Fatal error: Trait method M1 has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d
