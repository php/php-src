--TEST--
Bug #27669 (PHP 5 didn't support all possibilities for calling static methods dynamically)
--FILE--
<?php
	class A {
		function hello() {
			echo "Hello World\n";
		}
	}
	$y[0] = 'hello';
	A::{$y[0]}();
?>
===DONE===
--EXPECTF--
Deprecated: Non-static method A::hello() should not be called statically in %s on line %d
Hello World
===DONE===
