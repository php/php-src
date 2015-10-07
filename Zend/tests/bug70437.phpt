--TEST--
Bug #70437 (Changes to static properties in trait methods are not reflected in the class)
--FILE--
<?php

trait foo {
	static $bar = 1;

	public static function baz() {
		var_dump(++self::$bar);
		var_dump(++static::$bar);
	}
}

class bar {
	use foo { baz as private foo; }

	public static function baz() {
		self::foo();
		try {
			foo::baz();
		} catch (Error $e) {
			print $e->getMessage()."\n";
		}
		++self::$bar;
		var_dump(self::$bar);
		try {
			var_dump(foo::$bar);
		} catch (Error $e) {
			print $e->getMessage()."\n";
		}
	}
}

bar::baz();

?>
--EXPECT--
int(2)
int(3)
Cannot call abstract method foo::baz()
int(4)
Cannot access abstract property foo::$bar
