--TEST--
ZE2 A final class cannot be inherited
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

final class base {
	function show() {
		echo "base\n";
	}
}

$t = new base();

class derived extends base {
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Class derived may not inherit from final class (base) in %s on line %d
