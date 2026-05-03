--TEST--
Friends: friendship is not inherited
--FILE--
<?php

class Foo {
	friend Bar;

	protected static function protectedStatic() {
		echo __METHOD__ . "()\n";
	}


}

class Bar {

	public static function testFooAccess() {
		Foo::protectedStatic();
	}
}

class Baz extends Bar {

	public static function testFooAccessAgain() {
		Foo::protectedStatic();
	}
}

Bar::testFooAccess();
// Works when the method on the friend is inherited
Baz::testFooAccess();

echo "\n\n-----\n\n";

Baz::testFooAccessAgain();

?>
--EXPECTF--
Foo::protectedStatic()
Foo::protectedStatic()


-----


Fatal error: Uncaught Error: Call to protected method Foo::protectedStatic() from scope Baz in %s:%d
Stack trace:
#0 %s(%d): Baz::testFooAccessAgain()
#1 {main}
  thrown in %s on line %d
