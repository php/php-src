--TEST--
Bug #72920 (Accessing a private constant using constant() creates an exception AND warning)
--FILE--
<?php
class Foo {
	private const C1 = "a";
}

try {
    constant('Foo::C1');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
--EXPECT--
Couldn't find constant Foo::C1
