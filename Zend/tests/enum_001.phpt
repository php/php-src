--TEST--
Simple enum class
--FILE--
<?php

enum foo {
	a,
	b,
	c,
}

enum bar { a, d }

var_dump(foo::a, bar::a, bar::d);

?>
--EXPECT--
enum(foo::a)
enum(bar::a)
enum(bar::d)
