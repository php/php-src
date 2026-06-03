--TEST--
Friends: allows access to protected constants
--FILE--
<?php

class Foo {
	friend Bar;

	protected const FIRST = 1;
	private const SECOND = 2;
}

class Bar {
	public static function testConstantAccess() {
		var_dump(Foo::FIRST);
		echo "\n";
		try {
			var_dump(Foo::SECOND);
		} catch (Error $e) {
			echo $e . "\n";
		}
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	var_dump(Foo::FIRST);
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	var_dump(Foo::SECOND);
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testConstantAccess();

?>
--EXPECTF--
Error: Cannot access protected constant Foo::FIRST in %s:%d
Stack trace:
#0 {main}

Error: Cannot access private constant Foo::SECOND in %s:%d
Stack trace:
#0 {main}



-----

int(1)

Error: Cannot access private constant Foo::SECOND in %s:%d
Stack trace:
#0 %s(%d): Bar::testConstantAccess()
#1 {main}
