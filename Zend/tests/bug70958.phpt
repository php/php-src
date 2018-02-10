--TEST--
Bug #70958 (Invalid opcode while using ::class as trait method parameter default value)
--FILE--
<?php
trait Foo
{
	function bar($a = self::class) {
		var_dump($a);
	}
}

class B {
	use Foo;
}

$b = new B;

$b->bar();
?>
--EXPECT--
string(1) "B"
