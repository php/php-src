--TEST--
Memory leaks occur within an overloaded method that returns a reference
--SKIPIF--
<?php
	if (!function_exists('overload')) die('SKIP overload() not available');
?>
--FILE--
<?php
class foo {
	function foo() {
	}
	function __call($m, $a, &$r) {
		return true;
	}
	function &bar() {
		global $g;
		var_dump($g);
		return $g;
	}
}

$g = 456;

overload('foo');

$a = new foo();
var_dump($a->bar());
?>
--EXPECT--
int(456)
int(456)
