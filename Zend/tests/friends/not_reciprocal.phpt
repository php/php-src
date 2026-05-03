--TEST--
Friends: friendship is not reciprocal
--FILE--
<?php

class Foo {
	friend Bar;

	private static function privateStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testBarAccess() {
		Bar::privateStatic();
	}

}

class Bar {

	private static function privateStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testFooAccess() {
		Foo::privateStatic();
	}
}

Bar::testFooAccess();

echo "\n\n-----\n\n";

Foo::testBarAccess();

?>
--EXPECTF--
Foo::privateStatic()


-----


Fatal error: Uncaught Error: Call to private method Bar::privateStatic() from scope Foo in %s:%d
Stack trace:
#0 %s(%d): Foo::testBarAccess()
#1 {main}
  thrown in %s on line %d
