--TEST--
Test array_walk_recursive()
--FILE--
<?php
function foo($value) {
	echo $value . " foo\n";
}

function bar($value) {
	echo $value . " bar\n";
}

$arr = array (1,2,3);
var_dump (array_walk_recursive ($arr, 'foo'));
var_dump (array_walk_recursive ($arr, 'bar'));

?>
--EXPECTF--
1 foo
2 foo
3 foo
bool(true)
1 bar
2 bar
3 bar
bool(true)
