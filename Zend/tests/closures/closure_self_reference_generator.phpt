--TEST--
Testing Closure self-reference functionality: Generator
--FILE--
<?php

$fn = function(int $max) as $fn {
    yield from range($max, 0, -1);
    if ($max > 1) {
        yield from $fn($max - 1);
    }
};
print_r(iterator_to_array($fn(2), false));

?>
--EXPECT--
Array
(
    [0] => 2
    [1] => 1
    [2] => 0
    [3] => 1
    [4] => 0
)
