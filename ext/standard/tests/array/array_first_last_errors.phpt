--TEST--
array_first()/array_last() error cases
--FILE--
<?php
var_dump(array_first([]));
var_dump(array_last([]));

$array = [1, 2, 3];
unset($array[0]);
unset($array[1]);
unset($array[2]);

var_dump(array_first($array));
var_dump(array_last($array));
?>
--EXPECT--
NULL
NULL
NULL
NULL
