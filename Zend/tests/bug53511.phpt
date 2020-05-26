--TEST--
Bug #53511 (Exceptions are lost in case an exception is thrown in catch operator)
--FILE--
<?php
class Foo {
	function __destruct() {
		throw new Exception("ops 1");
	}
}

function test() {
	$e = new Foo();
	try {
		throw new Exception("ops 2");
	} catch (Exception $e) {
		echo $e->getMessage()."\n";
	}
}

test();
echo "bug\n";
--EXPECTF--
Fatal error: Uncaught Exception: ops 1 in %sbug53511.php:4
Stack trace:
#0 %sbug53511.php(12): Foo->__destruct()
#1 %sbug53511.php(17): test()
#2 {main}
  thrown in %sbug53511.php on line 4
