--TEST--
ZE2 An abstract method may not be called
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

abstract class fail {
	abstract function show();
}

class pass extends fail {
	function show() {
		echo "Call to function show()\n";
	}
	function error() {
		parent::show();
	}
}

$t = new pass();
$t->show();
$t->error();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Cannot call abstract method fail::show() in %s on line %d
