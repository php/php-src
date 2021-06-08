--TEST--
Test reduce() function
--FILE--
<?php

use function iterable\reduce;

/*
    Prototype: mixed iterable\reduce(array $array, callable($carry, $item): mixed $callback);
    Description: Iterate over iterable and reduce
*/

function dump_reduce(...$args) {
    try {
        var_dump(reduce(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

// The result of strtolower is locale-dependent, meaning that it cannot be converted to a constant by opcache.
dump_reduce([]);
dump_reduce([], function () {});
dump_reduce(['x', 'y', 'z'], function ($carry, $item) { $carry .= $item; return $carry; });
dump_reduce([strtolower('WORLD'), '!'], function ($carry, $item) { $carry .= $item; return $carry; });
dump_reduce([strtolower('WORLD')], function (string $carry, string $item): string { return $carry . $item; });

?>
--EXPECT--
Caught ArgumentCountError: iterable\reduce() expects exactly 2 arguments, 1 given
Caught ValueError: iterable\reduce(): Argument #1 ($iterable) must not be empty
string(3) "xyz"
string(6) "world!"
string(5) "world"
