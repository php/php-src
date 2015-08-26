--TEST--
ZE2 An abstract class cannot be instanciated
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

abstract class base {
	function show() {
		echo "base\n";
	}
}

class derived extends base {
}

$t = new derived();
$t->show();

$t = new base();
$t->show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
base

Fatal error: Uncaught Error: Cannot instantiate abstract class base in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
