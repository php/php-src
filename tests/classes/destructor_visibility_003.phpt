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
	public function __destruct() {
		echo __METHOD__ . "\n";
	}
}

$obj = new Derived;

unset($obj); // Derived::__destruct is being called not Base::__destruct

?>
===DONE===
--EXPECTF--
Derived::__destruct
===DONE===
