--TEST--
Friends: same friend cannot be used multiple times (case insensitive)
--FILE--
<?php

class Foo {
	friend Bar;
	friend BAR;
}

?>
--EXPECTF--
Fatal error: Cannot add BAR as a friend of Foo multiple times. in %s on line %d
