--TEST--
SPL FixedArray can reduce size of array
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
$array[0] = 'a';
$array[1] = 'b';
$array[2] = 'c';
$array[3] = 'd';
$array[4] = 'e';
$array->setSize(3);
print_r($array);
?>
--EXPECT--
SplFixedArray Object
(
    [0] => a
    [1] => b
    [2] => c
)
