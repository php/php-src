--TEST--
Bug #72441 (Segmentation fault: RFC list_keys)
--FILE--
<?php

$array = [];

list(
    '' => $foo,
    $bar
) = $array;
?>
--EXPECTF--
Fatal error: Cannot mix array elements with and without keys in assignments in %s on line %d
