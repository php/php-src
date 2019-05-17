--TEST--
SplFixedArray::setSize() grow
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

echo "\n";

$array = new SplFixedArray(2);

$array[0] = "Value 1";
$array[1] = "Value 2";

$array->setSize(4);

$array[2] = "Value 3";
$array[3] = "Value 4";

print_r($array);

?>
--EXPECT--
SplFixedArray Object
(
    [0] => Value 1
    [1] => Value 2
    [2] => Value 3
    [3] => Value 4
)
