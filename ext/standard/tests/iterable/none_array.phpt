--TEST--
Test none() function
--FILE--
<?php

use function iterable\none;

/*
	Prototype: bool none(array $iterable, mixed $callback);
	Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($nr)
{
	return is_int($nr);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

function dump_none(...$args) {
    try {
        var_dump(none(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

dump_none();
dump_none(true);
dump_none([]);
dump_none(true, function () {});
dump_none([], true);

echo "\n*** Testing basic functionality ***\n";

dump_none(['hello', 'world'], 'is_int_ex');
dump_none(['hello', 1, 2, 3], 'is_int_ex');
$iterations = 0;
dump_none(['hello', 1, 2, 3], function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing second argument to predicate ***\n";

dump_none([1, 2, 3], function($item, $key) {
	var_dump($key);
	return false;
});

echo "\n*** Testing edge cases ***\n";

dump_none([], 'is_int_ex');

dump_none(['key' => 'x'], null);

echo "\nDone";
?>
--EXPECT--
*** Testing not enough or wrong arguments ***
Caught ArgumentCountError: iterable\none() expects at least 1 argument, 0 given
Caught TypeError: iterable\none(): Argument #1 ($iterable) must be of type iterable, bool given
bool(true)
Caught TypeError: iterable\none(): Argument #1 ($iterable) must be of type iterable, bool given
Caught TypeError: iterable\none(): Argument #2 ($callback) must be a valid callback or null, no array or string given

*** Testing basic functionality ***
bool(true)
bool(false)
bool(false)
int(2)

*** Testing second argument to predicate ***
Caught ArgumentCountError: Too few arguments to function {closure}(), 1 passed and exactly 2 expected

*** Testing edge cases ***
bool(true)
bool(false)

Done
