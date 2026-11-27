--TEST--
Friends: 'self' cannot be used as a friend name
--FILE--
<?php

class Foo {
	friend self;
}

?>
--EXPECTF--
Fatal error: Cannot use "self" as friend name, as it is reserved in %s on line %d
