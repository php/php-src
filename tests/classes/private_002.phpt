--TEST--
ZE2 A private method cannot be called in another class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	private static function show() {
		echo "Call pass::show()\n";
	}

	public static function do_show() {
		pass::show();
	}
}

pass::do_show();

class fail {
	public static function show() {
		echo "Call fail::show()\n";
		pass::show();
	}
}

fail::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call pass::show()
Call fail::show()

Fatal error: Call to private method pass::show() from context 'fail' in %s on line %d
