--TEST--
ZE2 A protected method can only be called inside the class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	protected static function fail() {
		echo "Call fail()\n";
	}

	public static function good() {
		pass::fail();
	}
}

pass::good();
pass::fail();// must fail because we are calling from outside of class pass

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call fail()

Fatal error: Uncaught Error: Call to protected method pass::fail() from context '' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
