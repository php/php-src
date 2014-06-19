--TEST--
ZE2 Ensuring destructor visibility
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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
