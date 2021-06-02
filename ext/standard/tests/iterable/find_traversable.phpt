--TEST--
Test find() function
--FILE--
<?php

use function iterable\find;

/*
    Prototype: mixed iterable\find(array $array, callable($item): bool $callback, mixed $default = null);
    Description: Iterate over iterable and either return the first element matching $callback or the default.
*/

function dump_find(...$args) {
    try {
        $result = find(...$args);
        echo "Result: ";
        var_dump($result);
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

var_dump(find(range(1, 5), fn(int $x): bool => $x * $x === 9));

// The result of strtolower/strtoupper is locale-dependent, meaning that it cannot be converted to a constant by opcache.
dump_find(new ArrayObject([]));
dump_find(new ArrayObject([]), function ($value) { return false; }, strtoupper('default'));
dump_find(new ArrayObject(explode(' ', strtolower('ABCDEF GHIJK LMNOP QRSTUV'))), function (string $item): bool {
    var_dump($item); return false;
}, strtolower('DEFAULT'));

dump_find(new ArrayObject(explode(' ', strtolower('ABCDEF GHIJK'))), function (string $item): bool {
    var_dump($item); return $item === 'ghijk';
}, strtolower('DEFAULT'));

?>
--EXPECT--
int(3)
Caught ArgumentCountError: iterable\find() expects at least 2 arguments, 1 given
Result: string(7) "DEFAULT"
string(6) "abcdef"
string(5) "ghijk"
string(5) "lmnop"
string(6) "qrstuv"
Result: string(7) "default"
string(6) "abcdef"
string(5) "ghijk"
Result: string(5) "ghijk"