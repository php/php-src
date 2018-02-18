--TEST--
Immutable modifier is supported on classes.
--FILE--
<?php
immutable abstract class A {
	public function __construct() {
		echo "Ok\n";
	}
}
immutable class B extends A {}
immutable final class C extends B {}
new C;
?>
--EXPECT--

Ok
