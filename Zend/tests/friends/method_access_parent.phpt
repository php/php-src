--TEST--
Friends: allows access to methods as a `parent::` invocation
--FILE--
<?php

class Foo {
	friend Bar;

	private static function privateStatic() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}

	private function privateInstance() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
	}
}

class Bar {
	public static function privateStatic() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
		echo "Now calling parent...\n";
		parent::privateStatic();
		echo "...done\n";
	}

	public function privateInstance() {
		echo __METHOD__ . "() called, backtrace:\n";
		debug_print_backtrace();
		echo "Now calling parent...\n";
		parent::privateInstance();
		echo "...done\n";
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	Foo::privateStatic();
} catch (Error $e) {
	echo $e . "\n\n";
}
$f = new Foo();
try {
	$f->privateInstance();
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::privateStatic();
$b = new Bar();
$b->privateInstance();

?>
--EXPECTF--
Fatal error: Cannot use "parent" when current class scope has no parent in %s on line %d
