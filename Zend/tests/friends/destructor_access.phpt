--TEST--
Friends: allows access to destructors
--FILE--
<?php

class ProtectedDestructor {
	friend Bar;

	protected function __destruct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class PrivateDestructor {
	friend Bar;

	private function __destruct() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class Bar {
	public static function testDestructorAccess() {
		new ProtectedDestructor();
		echo "\n";
		try {
			new PrivateDestructor();
		} catch (Error $e) {
			echo $e . "\n";
		}
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	new ProtectedDestructor();
} catch (Error $e) {
	echo $e . "\n";
}
try {
	new PrivateDestructor();
} catch (Error $e) {
	echo $e . "\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testDestructorAccess();

?>
--EXPECTF--
Error: Call to protected ProtectedDestructor::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}
Error: Call to private PrivateDestructor::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}


-----

ProtectedDestructor::__destruct() called, backtrace:
#0 %s(%d): ProtectedDestructor->__destruct()
#1 %s(%d): Bar::testDestructorAccess()

Error: Call to private PrivateDestructor::__destruct() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testDestructorAccess()
#1 {main}
