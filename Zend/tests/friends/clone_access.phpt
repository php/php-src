--TEST--
Friends: allows access to cloning
--FILE--
<?php

class Foo {
	friend Bar;

	private function __clone() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class Bar {
	public static function testCloneAccess() {
		$f = new Foo();
		$g = clone $f;
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
$f = new Foo();
try {
	clone $f;
} catch (Error $e) {
	echo $e . "\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testCloneAccess();

?>
--EXPECTF--
Error: Call to private method Foo::__clone() from global scope in %s:%d
Stack trace:
#0 {main}


-----

Foo::__clone() called, backtrace:
#0 %s(%d): Foo->__clone()
#1 %s(%d): Bar::testCloneAccess()
