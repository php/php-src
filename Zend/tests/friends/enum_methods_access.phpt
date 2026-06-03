--TEST--
Friends: allows access to protected methods on enums
--FILE--
<?php

enum Foo {
	friend Bar;

	case First;
	case Second;

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
}

class Bar {
	public static function testMethodAccess() {
		Foo::protectedStatic();
		echo "\n";
		try {
			Foo::privateStatic();
		} catch (Error $e) {
			echo $e . "\n";
		}
		$f = Foo::First;
		$f->protectedInstance();
		echo "\n";
		try {
			$f->privateInstance();
		} catch (Error $e) {
			echo $e . "\n";
		}
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	Foo::protectedStatic();
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	Foo::privateStatic();
} catch (Error $e) {
	echo $e . "\n\n";
}
$f = Foo::First;
try {
	$f->protectedInstance();
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	$f->privateInstance();
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testMethodAccess();

?>
--EXPECTF--
Error: Call to protected method Foo::protectedStatic() from global scope in %s:%d
Stack trace:
#0 {main}

Error: Call to private method Foo::privateStatic() from global scope in %s:%d
Stack trace:
#0 {main}

Error: Call to protected method Foo::protectedInstance() from global scope in %s:%d
Stack trace:
#0 {main}

Error: Call to private method Foo::privateInstance() from global scope in %s:%d
Stack trace:
#0 {main}



-----

Foo::protectedStatic() called, backtrace:
#0 %s(%d): Foo::protectedStatic()
#1 %s(%d): Bar::testMethodAccess()

Error: Call to private method Foo::privateStatic() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testMethodAccess()
#1 {main}
Foo::protectedInstance() called, backtrace:
#0 %s(%d): Foo->protectedInstance()
#1 %s(%d): Bar::testMethodAccess()

Error: Call to private method Foo::privateInstance() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testMethodAccess()
#1 {main}
