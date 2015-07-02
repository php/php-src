--TEST--
Test array_every() function
--FILE--
<?php
/* 
	Prototype: bool array_every(array $array, mixed $callbac);
	Description: Iterate array and stop based on return value of callback
*/

echo "\n*** Testing not enough or wrong arguments ***\n";

var_dump(array_every());
var_dump(array_every(true));
var_dump(array_every([]));

echo "\n*** Testing basic functionality ***\n";

var_dump(array_every(array(1, 2, 3), 'is_int'));
var_dump(array_every(array('hello', 1, 2, 3), 'is_int'));
$iterations = 0;
var_dump(array_every(array('hello', 1, 2, 3), function($item) use (&$iterations) {
        ++$iterations;
        return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing edge cases ***\n";

var_dump(array_every(array(), 'is_int'));

echo "\nDone";
?> 
--EXPECTF--
*** Testing not enough or wrong arguments ***

Warning: array_every() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: array_every() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: array_every() expects exactly 2 parameters, 1 given in %s on line %d
NULL

*** Testing basic functionality ***
bool(true)
bool(false)
bool(false)
int(1)

*** Testing edge cases ***
bool(true)

Done
