--TEST--
SPL FixedArray offsetUnset throws error on no parameter
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$a = $array->offsetUnset();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplFixedArray::offsetUnset() expects exactly 1 parameter, 0 given in %s on line %d
PASS
