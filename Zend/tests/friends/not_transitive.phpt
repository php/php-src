--TEST--
Friends: friendship is not transitive
--FILE--
<?php

class Foo {
	friend Bar;

	protected static function protectedStatic() {
		echo __METHOD__ . "()\n";
	}


}

class Bar {
	friend Baz;

	protected static function protectedStatic() {
		echo __METHOD__ . "()\n";
	}

	public static function testFooAccess() {
		Foo::protectedStatic();
	}
}

class Baz {

	public static function testBarAccess() {
		Bar::protectedStatic();
	}

	public static function testFooAccess() {
		Foo::protectedStatic();
	}
}

Bar::testFooAccess();
Baz::testBarAccess();

echo "\n\n-----\n\n";

Baz::testFooAccess();

?>
--EXPECTF--
Foo::protectedStatic()
Bar::protectedStatic()


-----


Fatal error: Uncaught Error: Call to protected method Foo::protectedStatic() from scope Baz in %s:%d
Stack trace:
#0 %s(%d): Baz::testFooAccess()
#1 {main}
  thrown in %s on line %d
