--TEST--
ZE2 A static abstrcat method may not be called
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

abstract class fail {
	abstract static function show();
}

class pass extends fail {
	static function show() {
		echo "Call to function show()\n";
	}
}

pass::show();
fail::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Cannot call abstract method fail::show() in %s on line %d
