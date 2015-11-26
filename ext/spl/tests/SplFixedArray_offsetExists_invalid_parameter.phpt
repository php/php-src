--TEST--
SPL FixedArray offsetExists throws error only one parameter
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$a = $array->offsetExists();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplFixedArray::offsetExists() expects exactly 1 parameter, 0 given in %s on line %d
PASS
