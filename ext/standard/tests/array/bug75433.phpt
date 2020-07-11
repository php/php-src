--TEST--
array_values() preserves next index from source array when shallow-copying
--FILE--
<?php

$a = [1,2,3];
unset($a[2]);
$b = array_values($a);
$b[] = 4;
print_r($b);
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 4
)
