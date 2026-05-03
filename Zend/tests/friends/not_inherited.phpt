--TEST--
Friends: friendship is not inherited
--FILE--
<?php

class Foo {
	friend Bar;

	private static function privateStatic() {
		echo __METHOD__ . "()\n";
	}


}

class Bar {

	public static function testFooAccess() {
		Foo::privateStatic();
	}
}

class Baz extends Bar {

	public static function testFooAccessAgain() {
		Foo::privateStatic();
	}
}

Bar::testFooAccess();
// Works when the method on the friend is inherited
Baz::testFooAccess();

echo "\n\n-----\n\n";

Baz::testFooAccessAgain();

?>
--EXPECTF--
Foo::privateStatic()
Foo::privateStatic()


-----


Fatal error: Uncaught Error: Call to private method Foo::privateStatic() from scope Baz in %s:%d
Stack trace:
#0 %s(%d): Baz::testFooAccessAgain()
#1 {main}
  thrown in %s on line %d
