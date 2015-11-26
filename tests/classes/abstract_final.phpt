--TEST--
ZE2 A final method cannot be abstract
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class fail {
	abstract final function show();
}

echo "Done\n"; // Shouldn't be displayed
?>
--EXPECTF--

Fatal error: Cannot use the final modifier on an abstract class member in %s on line %d
