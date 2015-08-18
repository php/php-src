--TEST--
Bug #27669 (PHP 5 didn't support all possibilities for calling static methods dynamically)
--FILE--
<?php
	error_reporting(E_ALL & !E_STRICT);

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
Hello World
===DONE===
