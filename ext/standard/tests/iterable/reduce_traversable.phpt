--TEST--
Test reduce() function on Traversable
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

function generate_strings() {
    yield strtoupper('Hello');
    yield ' ';
    yield strtoupper('World!');
    return strtoupper('UNUSED');
}

// The result of strtolower is locale-dependent, meaning that it cannot be converted to a constant by opcache. Also, test reference counting.
dump_reduce(new ArrayObject([]));
dump_reduce(new ArrayObject([]), function () {});
dump_reduce(new ArrayObject(['x', 'y', 'z']), function ($carry, $item) { $carry .= $item; return $carry; });
dump_reduce(new ArrayObject([strtolower('WORLD'), '!']), function ($carry, $item) { $carry .= $item; return $carry; });
dump_reduce(new ArrayObject([strtolower('WORLD')]), function (string $carry, string $item): string { return $carry . $item; });
dump_reduce(generate_strings(), function (string $carry, string $item): string { return $carry . $item; });
dump_reduce(generate_strings(), function ($carry, $item): string { $item = $carry . $item; unset($carry);return $item; });
// Passing by reference is not supported.
dump_reduce(generate_strings(), function (string &$carry, string $item): string { $carry .= $item; return $carry;});

?>
--EXPECTF--
Caught ArgumentCountError: iterable\reduce() expects exactly 2 arguments, 1 given
Caught ValueError: iterable\reduce(): Argument #1 ($iterable) must not be empty
string(3) "xyz"
string(6) "world!"
string(5) "world"
string(12) "HELLO WORLD!"
string(12) "HELLO WORLD!"

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in /Users/levi.morrison/Projects/php/branch-master/ext/standard/tests/iterable/reduce_traversable.php on line 12

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in /Users/levi.morrison/Projects/php/branch-master/ext/standard/tests/iterable/reduce_traversable.php on line 12
string(12) "HELLO WORLD!"
