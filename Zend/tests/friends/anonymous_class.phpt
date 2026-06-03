--TEST--
Friends: anonymous classes can have friends
--FILE--
<?php

class Bar {
	public static function testAccess(object $obj) {
		$obj::protectedStatic();
		echo "\n";
		try {
			$obj::privateStatic();
		} catch (Error $e) {
			echo $e . "\n\n";
		}
		$obj->protectedInstance();
		echo "\n";
		try {
			$obj->privateInstance();
		} catch (Error $e) {
			echo $e . "\n\n";
		}
		$obj->protectedProp = 1;
		try {
			$obj->privateProp = 2;
		} catch (Error $e) {
			echo $e . "\n\n";
		}
		var_dump($obj);
		var_dump($obj::FIRST);
		try {
			var_dump($obj::SECOND);
			$obj::privateStatic();
		} catch (Error $e) {
			echo $e . "\n";
		}
	}
}

$obj = new class () {
	friend Bar;

	protected $protectedProp;
	private $privateProp;

	protected const FIRST = 1;
	private const SECOND = 2;

	protected static function protectedStatic() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

	private static function privateStatic() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

	protected function protectedInstance() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

	private function privateInstance() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}
};

Bar::testAccess($obj);

?>
--EXPECTF--
class@anonymous%s:%d$0::protectedStatic() called, backtrace:
#0 %s(%d): class@anonymous::protectedStatic()
#1 %s(%d): Bar::testAccess(Object(class@anonymous))

Error: Call to private method class@anonymous::privateStatic() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testAccess(Object(class@anonymous))
#1 {main}

class@anonymous%s:%d$0::protectedInstance() called, backtrace:
#0 %s(%d): class@anonymous->protectedInstance()
#1 %s(%d): Bar::testAccess(Object(class@anonymous))

Error: Call to private method class@anonymous::privateInstance() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testAccess(Object(class@anonymous))
#1 {main}

Error: Cannot access private property class@anonymous::$privateProp in %s:%d
Stack trace:
#0 %s(%d): Bar::testAccess(Object(class@anonymous))
#1 {main}

object(class@anonymous)#1 (2) {
  ["protectedProp":protected]=>
  int(1)
  ["privateProp":"class@anonymous":private]=>
  NULL
}
int(1)
Error: Cannot access private constant class@anonymous::SECOND in %s:%d
Stack trace:
#0 %s(%d): Bar::testAccess(Object(class@anonymous))
#1 {main}
