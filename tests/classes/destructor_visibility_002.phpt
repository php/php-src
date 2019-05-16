--TEST--
ZE2 Ensuring destructor visibility
--FILE--
<?php

class Base {
	private function __destruct() {
		echo __METHOD__ . "\n";
	}
}

class Derived extends Base {
}

$obj = new Derived;

?>
===DONE===
--EXPECTF--
===DONE===

Warning: Call to private Derived::__destruct() from context '' during shutdown ignored in Unknown on line %d
