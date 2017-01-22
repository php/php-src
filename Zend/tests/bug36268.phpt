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
Fatal error: Uncaught Error: Call to undefined function bar() in %sbug36268.php:8
Stack trace:
#0 {main}
  thrown in %sbug36268.php on line 8
