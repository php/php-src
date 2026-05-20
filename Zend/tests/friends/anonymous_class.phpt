--TEST--
Friends: anonymous classes can have friends
--FILE--
<?php

class Bar {
	public static function testAccess(object $obj) {
		$obj::protectedStatic();
		echo "\n";
		$obj::privateStatic();
		echo "\n";
		$obj->protectedInstance();
		echo "\n";
		$obj->privateInstance();
		echo "\n";
		$obj->protectedProp = 1;
		$obj->privateProp = 2;
		var_dump($obj);
		var_dump($obj::FIRST);
		var_dump($obj::SECOND);
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

class@anonymous%s:%d$0::privateStatic() called, backtrace:
#0 %s(%d): class@anonymous::privateStatic()
#1 %s(%d): Bar::testAccess(Object(class@anonymous))

class@anonymous%s:%d$0::protectedInstance() called, backtrace:
#0 %s(%d): class@anonymous->protectedInstance()
#1 %s(%d): Bar::testAccess(Object(class@anonymous))

class@anonymous%s:%d$0::privateInstance() called, backtrace:
#0 %s(%d): class@anonymous->privateInstance()
#1 %s(%d): Bar::testAccess(Object(class@anonymous))

object(class@anonymous)#1 (2) {
  ["protectedProp":protected]=>
  int(1)
  ["privateProp":"class@anonymous":private]=>
  int(2)
}
int(1)
int(2)
