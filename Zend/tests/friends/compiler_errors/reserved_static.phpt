--TEST--
Friends: 'static' cannot be used as a friend name
--FILE--
<?php

class Foo {
	friend static;
}

?>
--EXPECTF--
Fatal error: Cannot use "static" as friend name, as it is reserved in %s on line %d
