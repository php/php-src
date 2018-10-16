--TEST--
PHP 4 Constructors cannot declare a return type
--FILE--
<?php

class Foo {
	function foo() : Foo {}
}
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Foo has a deprecated constructor in %s on line %d

Fatal error: Constructor %s::%s() cannot declare a return type in %s on line %d
