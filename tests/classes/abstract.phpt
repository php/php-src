--TEST--
An abstract method may not be called
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class fail {
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

echo "Done\n"; // shouldn't be displayed of cause
?>
--EXPECTF--
Call to function show()

Fatal error: Cannot call abstract method fail::show() in %s on line %d
