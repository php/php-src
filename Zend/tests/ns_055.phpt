--TEST--
055: typehints in namespaces
--FILE--
<?php
namespace test\ns1;

class Foo {
	function test1(Foo $x) {
		echo "ok\n";
	}
	function test2(\test\ns1\Foo $x) {
		echo "ok\n";
	}
	function test3(\Exception $x) {
		echo "ok\n";
	}
}

$foo = new Foo();
$ex = new \Exception();
$foo->test1($foo);
$foo->test2($foo);
$foo->test3($ex);
?>
--EXPECT--
ok
ok
ok
