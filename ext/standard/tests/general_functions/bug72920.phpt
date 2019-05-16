--TEST--
Bug #72920 (Accessing a private constant using constant() creates an exception AND warning)
--FILE--
<?php
class Foo {
	private const C1 = "a";
}

var_dump(constant('Foo::C1'));
--EXPECTF--
Warning: constant(): Couldn't find constant Foo::C1 in %s on line %d
NULL
