--TEST--
Short functions that yield.
--FILE--
<?php

function read($it): iterable => yield from $it;

$a = [1, 2, 3];

print_r(iterator_to_array(read($a)));

?>
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
