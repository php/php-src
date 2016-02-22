--TEST--
Checking error message when the trait doesn't implements the interface
--FILE--
<?php

interface baz {
	public function abc();
}

trait foo implements baz {
}

?>
--EXPECTF--
Fatal error: Trait foo contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (baz::abc) in %s on line %d
