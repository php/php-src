--TEST--
SPL FixedArray offsetGet throws error on no parameter
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$array[0] = 'a';
$a = $array->offsetGet();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplFixedArray::offsetGet() expects exactly 1 parameter, 0 given in %s on line %d
PASS
