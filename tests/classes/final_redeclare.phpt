--TEST--
ZE2 A final method may not be overwritten
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	final function show() {
		echo "Call to function pass::show()\n";
	}
}

$t = new pass();
$t->show();

class fail extends pass {
	function show() {
		echo "Call to function fail::show()\n";
	}
}

echo "Done\n"; // Shouldn't be displayed
?>
--EXPECTF--
Call to function pass::show()

Fatal error: Cannot override final method pass::show() in %s on line %d
