--TEST--
Bug #23298 (serialize() and floats/doubles)
--FILE--
<?php 
	ini_set('precision', 12);
	$foo = 1.428571428571428647642857142;
	$bar = unserialize(serialize($foo));
	var_dump(($foo === $bar));
?>
--EXPECT--
bool(true)
