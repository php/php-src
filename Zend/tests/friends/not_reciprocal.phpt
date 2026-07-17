--TEST--
Friends: friendship is not reciprocal
--FILE--
<?php

class Foo {
	friend Bar;

	protected static function protectedStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testBarAccess() {
		Bar::protectedStatic();
	}

}

class Bar {

	protected static function protectedStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testFooAccess() {
		Foo::protectedStatic();
	}
}

Bar::testFooAccess();

echo "\n\n-----\n\n";

Foo::testBarAccess();

?>
--EXPECTF--
Foo::protectedStatic()


-----


Fatal error: Uncaught Error: Call to protected method Bar::protectedStatic() from scope Foo in %s:%d
Stack trace:
#0 %s(%d): Foo::testBarAccess()
#1 {main}
  thrown in %s on line %d
