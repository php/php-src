--TEST--
Friends: allows access to inherited non-overridden protected instance properties
--FILE--
<?php

class Foo {
	friend Bar;

	protected $protectedInstance;
	private $privateInstance;

	protected $protectedInstance2;
	private $privateInstance2;
}

class FooChild extends Foo {
	protected $protectedInstance2;
	private $privateInstance2;
}

class Bar {
	public static function testPropertyAccess() {
		$fc = new FooChild();
		$fc->protectedInstance = 1;
		try {
			$fc->privateInstance = 2;
		} catch (Error $e) {
			echo $e . "\n";
		}

		try {
			$fc->protectedInstance2 = 3;
		} catch (Error $e) {
			echo $e . "\n\n";
		}
		try {
			$fc->privateInstance2 = 4;
		} catch (Error $e) {
			echo $e . "\n\n";
		}

		var_dump($fc);
	}
}

Bar::testPropertyAccess();

?>
--EXPECTF--
Deprecated: Creation of dynamic property FooChild::$privateInstance is deprecated in %s on line %d
Error: Cannot access protected property FooChild::$protectedInstance2 in %s:%d
Stack trace:
#0 %s(%d): Bar::testPropertyAccess()
#1 {main}

Error: Cannot access private property FooChild::$privateInstance2 in %s:%d
Stack trace:
#0 %s(%d): Bar::testPropertyAccess()
#1 {main}

object(FooChild)#%d (6) {
  ["protectedInstance":protected]=>
  int(1)
  ["privateInstance":"Foo":private]=>
  NULL
  ["protectedInstance2":protected]=>
  NULL
  ["privateInstance2":"Foo":private]=>
  NULL
  ["privateInstance2":"FooChild":private]=>
  NULL
  ["privateInstance"]=>
  int(2)
}
