--TEST--
__clone cannot declare a return type

--FILE--
<?php

class Foo {
	function __clone() : Foo {}
}

--EXPECTF--
Fatal error: %s::%s() cannot declare a return type in %s on line %s
