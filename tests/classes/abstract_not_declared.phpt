--TEST--
ZE2 An abstract class must be declared abstract
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class fail {
	abstract function show();
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Class fail contains 1 abstract methods and must therefore be declared abstract (fail::show) in %s on line %d
