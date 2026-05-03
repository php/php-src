--TEST--
Friends: allows access to inherited non-overridden constants
--FILE--
<?php

class Foo {
	friend Bar;

	protected const FIRST = 1;
	protected const SECOND = 2;
}

class FooChild extends Foo {
	protected const SECOND = 3;
}

class Bar {
	public static function testConstantAccess() {
		var_dump(FooChild::FIRST);

		echo "\n";

		try {
			var_dump(FooChild::SECOND);
		} catch (Error $e) {
			echo $e . "\n\n";
		}
	}
}

Bar::testConstantAccess();

?>
--EXPECTF--
int(1)

Error: Cannot access protected constant FooChild::SECOND in %s:%d
Stack trace:
#0 %s(%d): Bar::testConstantAccess()
#1 {main}
