--TEST--
GH-14202: Fetch splat arg value by RW
--FILE--
<?php
$args = [1];
$ref = [&$args];
function test(&$v) {
	$v = 7;
};
test(...$ref[0]);
var_dump($args[0]);
?>
--EXPECT--
int(7)
