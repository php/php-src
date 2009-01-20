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
--EXPECTF--

Strict Standards: Only variables should be passed by reference in %sbug47165.php on line 11
ok
