--TEST--
A method cannot be redeclared abstrcat
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	function show() {
		echo "Call to function show()\n";
	}
}

class fail extends pass {
	abstract function show();
}

echo "Done\n"; // Shouldn't be displayed
?>
--EXPECTF--

Fatal error: Cannot make non abstract method pass::show() abstract in class fail in %s on line %d
