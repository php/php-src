--TEST--
Bug #40191 (use of array_unique() with objects triggers segfault)
--FILE--
<?php

$arrObj = new ArrayObject();
$arrObj->append('foo');
$arrObj->append('bar');
$arrObj->append('foo');

$arr = array_unique($arrObj);
var_dump($arr);

echo "Done\n";
?>
--EXPECTF--
Warning: array_unique() expects parameter 1 to be array, object given in %s on line %d
NULL
Done
