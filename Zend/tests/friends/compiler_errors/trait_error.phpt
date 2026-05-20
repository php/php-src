--TEST--
Friends: cannot be used on traits
--FILE--
<?php

trait Foo {
	friend Bar;
}

?>
--EXPECTF--
Fatal error: Cannot add friends to traits. Bar is used in Foo in %s on line %d
