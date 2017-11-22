--TEST--
ZE2 A private method cannot be called in a derived class
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

Fatal error: Uncaught Error: Call to private method pass::show() from context 'fail' in %s:%d
Stack trace:
#0 %s(%d): fail::not_ok()
#1 {main}
  thrown in %s on line %d
