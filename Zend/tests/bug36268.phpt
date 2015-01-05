--TEST--
Bug #36268 (Object destructors called even after fatal errors)
--FILE--
<?php
class Foo {
	function __destruct() {
		echo "Ha!\n";
	}
}
$x = new Foo();
bar();
?>
--EXPECTF--
Fatal error: Call to undefined function bar() in %sbug36268.php on line 8
