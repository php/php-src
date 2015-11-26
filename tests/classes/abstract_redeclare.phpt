--TEST--
ZE2 A method cannot be redeclared abstract
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

Fatal error: Class fail contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (fail::show) in %sabstract_redeclare.php on line %d
