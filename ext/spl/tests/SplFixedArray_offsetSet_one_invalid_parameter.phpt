--TEST--
SPL FixedArray offsetSet throws error only one parameter
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$a = $array->offsetSet(2);
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplFixedArray::offsetSet() expects exactly 2 parameters, 1 given in %s on line %d
PASS
