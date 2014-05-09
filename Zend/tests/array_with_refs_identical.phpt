--TEST--
Identical comparison of array with references
--FILE--
<?php

$foo = 42;
$array1 = [&$foo];
$array2 = [$foo];
var_dump($array1 === $array2);

?>
--EXPECT--
bool(true)
