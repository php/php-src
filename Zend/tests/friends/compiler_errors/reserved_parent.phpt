--TEST--
Friends: 'parent' cannot be used as a friend name
--FILE--
<?php

class Foo {
	friend parent;
}

?>
--EXPECTF--
Fatal error: Cannot use "parent" as friend name, as it is reserved in %s on line %d
