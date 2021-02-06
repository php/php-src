--TEST--
Test all() function
--FILE--
<?php

use function iterable\all;

/*
    Prototype: bool all(array $array, ?callable $callback = null, int $use_type = 0);
    Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($item)
{
    return is_int($item);
}

function dump_all(...$args) {
    try {
        var_dump(all(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}


echo "\n*** Testing not enough or wrong arguments ***\n";

dump_all(new ArrayIterator());
dump_all(new ArrayIterator(), true);

echo "\n*** Testing basic functionality ***\n";

dump_all(new ArrayIterator([1, 2, 3]), 'is_int_ex');
dump_all(new ArrayIterator(['hello', 1, 2, 3]), 'is_int_ex');
$iterations = 0;
dump_all(new ArrayIterator(['hello', 1, 2, 3]), function($item) use (&$iterations) {
    ++$iterations;
    return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing edge cases ***\n";

dump_all(new ArrayIterator(), 'is_int_ex');

echo "\nDone";
?>
--EXPECT--
*** Testing not enough or wrong arguments ***
bool(true)
Caught TypeError: iterable\all(): Argument #2 ($callback) must be a valid callback or null, no array or string given

*** Testing basic functionality ***
bool(true)
bool(false)
bool(false)
int(1)

*** Testing edge cases ***
bool(true)

Done