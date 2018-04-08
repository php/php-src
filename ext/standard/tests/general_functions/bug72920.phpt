--TEST--
Bug #72920 (Accessing a private constant using constant() creates an exception AND warning)
--FILE--
<?php
class Foo {
	private const C1 = "a";
}

try {
	var_dump(constant('Foo::C1'));
} catch (Error $e) {
	var_dump($e->getMessage());
}
--EXPECT--
string(35) "Cannot access private const Foo::C1"
