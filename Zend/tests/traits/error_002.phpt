--TEST--
Trying to use an undefined trait
--FILE--
<?php

class A {
	use abc;
}

?>
--EXPECTF--
Fatal error: Trait 'abc' not found in %s on line %d
