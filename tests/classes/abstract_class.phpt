--TEST--
ZE2 An abstract class cannot be instantiated
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
}

$t2 = new pass();
$t2->show();

$t = new fail();
$t->show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Cannot instantiate abstract class fail in %s on line %d
