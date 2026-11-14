--TEST--
Friends: cannot be used on interfaces
--FILE--
<?php

interface Foo {
	friend Bar;
}

?>
--EXPECTF--
Fatal error: Cannot add friends to interfaces. Bar is used in Foo in %s on line %d
