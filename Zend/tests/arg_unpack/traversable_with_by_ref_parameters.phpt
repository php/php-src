--TEST--
Traversables cannot be unpacked into by-reference parameters
--FILE--
<?php

function test($val1, $val2, $val3, &$ref) {
    $ref = 42;
}

function gen($array) {
    foreach ($array as $element) {
        yield $element;
    }
}

test(1, 2, 3, $b, ...gen([4, 5, 6]));
var_dump($b);

test(...gen([1, 2, 3, 4]));
test(1, 2, ...gen([3, 4]));
test(...gen([1, 2]), ...gen([3, 4]));

?>
--EXPECTF--
int(42)

Warning: Cannot pass by-reference argument 4 of test() by unpacking a Traversable, passing by-value instead in %s on line %d

Warning: Cannot pass by-reference argument 4 of test() by unpacking a Traversable, passing by-value instead in %s on line %d

Warning: Cannot pass by-reference argument 4 of test() by unpacking a Traversable, passing by-value instead in %s on line %d
