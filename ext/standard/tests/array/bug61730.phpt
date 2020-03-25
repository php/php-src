--TEST--
Bug #61730 (Segfault from array_walk modifying an array passed by reference)
--FILE--
<?php
$myArray = array_fill(0, 10, 1);

array_walk(
    $myArray,
    function($value, $key) use ($myArray)
    {
        reset($myArray);
    }
);

array_walk(
    $myArray,
    function($value, $key) use (&$myArray)
    {
        var_dump($key);
        unset($myArray[$key]);
        unset($myArray[$key+1]);
        unset($myArray[$key+2]);
    }
);



print_r($myArray);
--EXPECT--
int(0)
int(3)
int(6)
int(9)
Array
(
)
