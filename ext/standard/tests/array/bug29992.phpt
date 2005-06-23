--TEST--
Bug #29992 (foreach by reference corrupts the array)
--FILE--
<?php

$array = array(1,2,3);

print_r($array);

foreach($array as $item) var_dump($item);
foreach($array as &$item) var_dump($item);
foreach($array as &$item) var_dump($item);
foreach($array as $item) var_dump($item);

print_r($array);

?>
===DONE===
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
===DONE===
