--TEST--
Friends: friendship is not transitive
--FILE--
<?php

class Foo {
	friend Bar;

	private static function privateStatic() {
		echo __METHOD__ . "()\n";
	}


}

class Bar {
	friend Baz;

	private static function privateStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testFooAccess() {
		Foo::privateStatic();
	}
}

class Baz {

	public static function testBarAccess() {
		Bar::privateStatic();
	}

	public static function testFooAccess() {
		Foo::privateStatic();
	}
}

Bar::testFooAccess();
Baz::testBarAccess();

echo "\n\n-----\n\n";

Baz::testFooAccess();

?>
--EXPECTF--
Foo::privateStatic()
Bar::privateStatic()


-----


Fatal error: Uncaught Error: Call to private method Foo::privateStatic() from scope Baz in %s:%d
Stack trace:
#0 %s(%d): Baz::testFooAccess()
#1 {main}
  thrown in %s on line %d
