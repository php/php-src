--TEST--
GH-12468: Double-free of doc_comment when overriding static property via trait
--FILE--
<?php
trait T {
	/** some doc */
	static protected $a = 0;
}
class A {
	use T;
}
class B extends A {
	use T;
}
?>
===DONE===
--EXPECT--
===DONE===
