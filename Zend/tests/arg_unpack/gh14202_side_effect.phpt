--TEST--
GH-14202: Fetching splat arg value by RW has some side-effects
--FILE--
<?php
function test($byVal) {
	var_dump($byVal);
}
try {
	test(...$foo['bar']);
} catch (Error $e) {
	echo $e->getMessage(), "\n";
}
var_dump($foo);
?>
--EXPECTF--
Warning: Undefined variable $foo in %s on line %d

Warning: Undefined array key "bar" in %s on line %d
Only arrays and Traversables can be unpacked
array(1) {
  ["bar"]=>
  NULL
}
