--TEST--
Friends: same friend cannot be used multiple times
--FILE--
<?php

class Foo {
	friend Bar;
	friend Bar;
}

?>
--EXPECTF--
Fatal error: Cannot add Bar as a friend of Foo multiple times. in %s on line %d
