--TEST--
$this in eval() block
--FILE--
<?php
class C {
	function foo() {
		eval('var_dump($this);');
		eval('var_dump($this);');
	}
}
$x = new C;
$x->foo();
--EXPECT--
object(C)#1 (0) {
}
object(C)#1 (0) {
}
