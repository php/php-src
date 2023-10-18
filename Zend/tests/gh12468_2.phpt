--TEST--
GH-12468: Double-free of shared doc_comment in trait
--FILE--
<?php
trait T {
	/** some doc */
	static protected $a = 0;
}
class A {
	/** some doc */
	static protected $a = 0;
}
class B extends A {
	use T;
}
?>
===DONE===
--EXPECT--
===DONE===
