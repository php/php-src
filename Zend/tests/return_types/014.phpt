--TEST--
Constructors cannot declare a return type
--FILE--
<?php

class Foo {
	function __construct() : Foo {}
}
--EXPECTF--
Fatal error: Constructor %s::%s() cannot declare a return type in %s on line %s
