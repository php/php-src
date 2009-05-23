--TEST--
SPL FixedArray offsetSet throws error on no parameters
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$a = $array->offsetSet();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplFixedArray::offsetSet() expects exactly 2 parameters, 0 given in %s on line %d
PASS
