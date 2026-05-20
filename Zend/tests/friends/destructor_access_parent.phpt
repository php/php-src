--TEST--
Friends: allows access to destructors via `parent::`
--FILE--
<?php

class Foo {
	friend Bar;

	private function __destruct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class Bar extends Foo {

	public function __destruct() {
		parent::__destruct();
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	new Foo();
} catch (Error $e) {
	echo $e . "\n";
}

echo "\n\n-----\n\n";

// But friend works
new Bar();

?>
--EXPECTF--
Error: Call to private Foo::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}


-----

Foo::__destruct() called, backtrace:
#0 %s(%d): Foo->__destruct()
#1 %s(%d): Bar->__destruct()
