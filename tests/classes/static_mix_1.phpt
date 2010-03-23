--TEST--
ZE2 You cannot overload a static method with a non static method
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

echo "Done\n"; // shouldn't be displayed
?>                                                            
--EXPECTF--
Fatal error: Cannot make static method pass::show() non static in class fail in %s on line %d