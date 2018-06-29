--TEST--
Test mixed variance in inheritance.
--FILE--
<?php

class Foo {
	function test(mixed $arg): mixed
	{
		return $arg;
	}
}

class Bar extends Foo {
	function test($arg) {
		return parent::test($arg);
	}
}

class Baz extends bar {
	function test(mixed $arg): mixed {
		return parent::test($arg);
	}
}

var_dump((new Baz())->test(123));
--EXPECT--
int(123)
