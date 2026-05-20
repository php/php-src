--TEST--
Friends: allows access to constructor as a `parent::` invocation
--FILE--
<?php

class Foo {
	friend Bar;

	private function __construct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}
}

class Bar extends Foo {
	public function __construct() {
		parent::__construct();
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	new Foo();
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
new Bar();

?>
--EXPECTF--
Error: Call to private Foo::__construct() from global scope in %s:%d
Stack trace:
#0 {main}



-----

Foo::__construct() called, backtrace:
#0 %s(%d): Foo->__construct()
#1 %s(%d): Bar->__construct()
