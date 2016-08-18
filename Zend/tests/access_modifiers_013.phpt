--TEST--
Prevent abstract and final in the same class declaration
--FILE--
<?php

final abstract class C {
	private function priv() { }
}

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an abstract class in %s on line %d
