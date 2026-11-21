--TEST--
Friends: same friend cannot be used multiple times (after name resolution)
--FILE--
<?php

use SomeNamespace\Bar;
use SomeNamespace\Bar as Baz;

class Foo {
	friend Bar;
	friend Baz;
}

?>
--EXPECTF--
Fatal error: Cannot add SomeNamespace\Bar as a friend of Foo multiple times. in %s on line %d
