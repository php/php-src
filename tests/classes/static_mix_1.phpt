--TEST--
You cannot overload astatic methods with a non static methods
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	static function show() {
		echo "Call to function pass::show()\n";
	}
}

class fail extends pass {
	function show() {
		echo "Call to function fail::show()\n";
	}
}

pass::show();
fail::show();

echo "Done\n"; // shouldn't be displayed of cause
?>
--EXPECTF--
Fatal error: Cannot make static method pass::show() non static in class fail in %s on line %d