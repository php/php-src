--TEST--
Test array_any() function
--FILE--
<?php
/*
	Prototype: bool array_any(array $array, mixed $callback);
	Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($nr)
{
	return is_int($nr);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

var_dump(array_any());
var_dump(array_any(true));
var_dump(array_any([]));
var_dump(array_any(true, function () {}));
var_dump(array_any([], true));

echo "\n*** Testing basic functionality ***\n";

var_dump(array_any(['hello', 'world'], 'is_int_ex'));
var_dump(array_any(['hello', 1, 2, 3], 'is_int_ex'));
$iterations = 0;
var_dump(array_any(['hello', 1, 2, 3], function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing second argument to predicate ***\n";

var_dump(array_any([1, 2, 3], function($item, $key) {
	var_dump($key);
	return false;
}));

echo "\n*** Testing edge cases ***\n";

var_dump(array_any([], 'is_int_ex'));

echo "\nDone";
?>
--EXPECTF--
*** Testing not enough or wrong arguments ***

Warning: array_any() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: array_any() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: array_any() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: array_any() expects parameter 1 to be array, bool given in %s on line %d
NULL

Warning: array_any() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

*** Testing basic functionality ***
bool(false)
bool(true)
bool(true)
int(2)

*** Testing second argument to predicate ***
int(0)
int(1)
int(2)
bool(false)

*** Testing edge cases ***
bool(false)

Done
