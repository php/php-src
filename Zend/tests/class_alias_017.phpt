--TEST--
Testing alias with get_called_class() and get_class()
--FILE--
<?php

class foo {
	public function __construct() {
		echo get_called_class(), "\n";
	}
	static public function test() {
		echo get_class(), "\n";
	}
}

class_alias('foo', 'bar');

new bar;


class baz extends bar {
}

new baz;
baz::test();

bar::test();

?>
--EXPECT--
foo
baz
foo
foo
