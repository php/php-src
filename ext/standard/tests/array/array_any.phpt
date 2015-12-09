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

echo "\n*** Testing basic functionality ***\n";

var_dump(array_any(array('hello', 'world'), 'is_int_ex'));
var_dump(array_any(array('hello', 1, 2, 3), 'is_int_ex'));
$iterations = 0;
var_dump(array_any(array('hello', 1, 2, 3), function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing traversable functionality ***\n";

var_dump(array_any((function() {
	yield 'foo' => 'bar';
	yield 456;
})(), function($value, $key) {
	var_dump($value, $key);
	return true;
}));

echo "\n*** Testing edge cases ***\n";

var_dump(array_any(array(), 'is_int_ex'));

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

*** Testing basic functionality ***
bool(false)
bool(true)
bool(true)
int(2)

*** Testing traversable functionality ***
string(3) "bar"
string(3) "foo"
bool(true)

*** Testing edge cases ***
bool(false)

Done
