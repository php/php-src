--TEST--
Friends: allows access to constructor
--FILE--
<?php

class ConstructorProtected {
	friend Bar;

	protected function __construct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}
}

class ConstructorPrivate {
	friend Bar;

	private function __construct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}
}

class Bar {
	public static function testConstructorAccess() {
		new ConstructorProtected();
		echo "\n";
		new ConstructorPrivate();
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	new ConstructorProtected();
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	new ConstructorPrivate();
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testConstructorAccess();

?>
--EXPECTF--
Error: Call to protected ConstructorProtected::__construct() from global scope in %s:%d
Stack trace:
#0 {main}

Error: Call to private ConstructorPrivate::__construct() from global scope in %s:%d
Stack trace:
#0 {main}



-----

ConstructorProtected::__construct() called, backtrace:
#0 %s(%d): ConstructorProtected->__construct()
#1 %s(%d): Bar::testConstructorAccess()

ConstructorPrivate::__construct() called, backtrace:
#0 %s(%d): ConstructorPrivate->__construct()
#1 %s(%d): Bar::testConstructorAccess()
