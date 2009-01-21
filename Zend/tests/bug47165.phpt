--TEST--
Bug #47165 (Possible memory corruption when passing return value by reference)
--FILE--
<?php
class Foo {
	var $bar = array();

	static function bar() {
		static $instance = null;
		$instance = new Foo();
		return $instance->bar;
	}
}
extract(Foo::bar());
echo "ok\n";
?>
--EXPECT--
ok
