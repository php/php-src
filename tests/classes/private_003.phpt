--TEST--
A private method cannot be called in a derived class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
ini_set("error_reporting",2039);
class pass {
	private static function show() {
		echo "Call show()\n";
	}

	protected static function good() {
		pass::show();
	}
}

class fail extends pass {
	static function ok() {
		pass::good();
	}

	static function not_ok() {
		pass::show();
	}
}

fail::ok();
fail::not_ok(); // calling a private function

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Call to private method pass::show() from context 'fail' in %s on line %d
