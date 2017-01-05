--TEST--
Destructors cannot declare a return type

--FILE--
<?php

class Foo {
	function __destruct() : Foo {}
}

--EXPECTF--
Fatal error: Destructor %s::%s() cannot declare a return type in %s on line %s
