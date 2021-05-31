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
dump_reduce(new ArrayObject([]), function () {}, strtolower('TEST'));
dump_reduce(new ArrayObject(['x', 'y', 'z']), function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('TEST'));
dump_reduce(new ArrayObject([strtolower('WORLD'), '!']), function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('HELLO'));
dump_reduce(new ArrayObject([strtolower('WORLD')]), function (string $carry, string $item): string { return $carry . $item; }, strtolower('HELLO'));
dump_reduce(generate_strings(), function (string $carry, string $item): string { return $carry . $item; }, '');
dump_reduce(generate_strings(), function ($carry, $item): string { $item = $carry . $item; unset($carry);return $item; }, '');
// Passing by reference is not supported.
dump_reduce(generate_strings(), function (string &$carry, string $item): string { $carry .= $item; return $carry;}, '');

?>
--EXPECTF--
Caught ArgumentCountError: iterable\reduce() expects at least 2 arguments, 1 given
string(4) "test"
string(7) "testxyz"
string(11) "helloworld!"
string(10) "helloworld"
string(12) "HELLO WORLD!"
string(12) "HELLO WORLD!"

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12
string(12) "HELLO WORLD!"
