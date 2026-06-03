--TEST--
Friends: allows access to private __clone
--FILE--
<?php

class ProtectedClone {
	friend Bar;

	protected function __clone() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class PrivateClone {
	friend Bar;

	private function __clone() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

}

class Bar {
	public static function testCloneAccess() {
		$f = new ProtectedClone();
		$g = clone $f;
		echo "\n";

		$f = new PrivateClone();
		try {
			clone $f;
		} catch (Error $e) {
			echo $e . "\n";
		}
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
$f = new ProtectedClone();
try {
	clone $f;
} catch (Error $e) {
	echo $e . "\n\n";
}
$f = new PrivateClone();
try {
	clone $f;
} catch (Error $e) {
	echo $e;
}

echo "\n\n-----\n\n";

// But friend works
Bar::testCloneAccess();

?>
--EXPECTF--
Error: Call to protected method ProtectedClone::__clone() from global scope in %s:%d
Stack trace:
#0 {main}

Error: Call to private method PrivateClone::__clone() from global scope in %s:%d
Stack trace:
#0 {main}

-----

ProtectedClone::__clone() called, backtrace:
#0 %s(%d): ProtectedClone->__clone()
#1 %s(%d): Bar::testCloneAccess()

Error: Call to private method PrivateClone::__clone() from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testCloneAccess()
#1 {main}
