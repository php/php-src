--TEST--
Test array_some() and array_all() function
--FILE--
<?php
/* Prototype: bool array_some(array $array, mixed $callback);
   Prototype: bool array_all(array $array, mixed $callbac);
   Description: Iterate array and stop based on return value of callback
*/

echo "\n*** Testing array_some() with not enough or wrong arguments ***\n";

var_dump(array_some());
var_dump(array_some(true));
var_dump(array_some([]));

echo "\n*** Testing array_all() with not enough or wrong arguments ***\n";

var_dump(array_all());
var_dump(array_all(true));
var_dump(array_all([]));

echo "\n*** Testing array_some() basic functionality ***\n";

var_dump(array_some(array('hello', 'world'), 'is_int'));
var_dump(array_some(array('hello', 1, 2, 3), 'is_int'));
$iterations = 0;
var_dump(array_some(array('hello', 1, 2, 3), function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing array_all() basic functionality ***\n";

var_dump(array_all(array(1, 2, 3), 'is_int'));
var_dump(array_all(array('hello', 1, 2, 3), 'is_int'));
$iterations = 0;
var_dump(array_all(array('hello', 1, 2, 3), function($item) use (&$iterations) {
        ++$iterations;
        return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing array_some() / array_all() edge cases ***\n";

var_dump(array_all(array(), 'is_int'));
var_dump(array_some(array(), 'is_int'));

echo "\nDone";
?> 
--EXPECTF--

*** Testing array_some() with not enough or wrong arguments ***

Warning: array_some() expects exactly 2 parameters, 0 given in %s/array_some_all.php on line 9
NULL

Warning: array_some() expects exactly 2 parameters, 1 given in %s/array_some_all.php on line 10
NULL

Warning: array_some() expects exactly 2 parameters, 1 given in %s/array_some_all.php on line 11
NULL

*** Testing array_all() with not enough or wrong arguments ***

Warning: array_all() expects exactly 2 parameters, 0 given in %s/array_some_all.php on line 15
NULL

Warning: array_all() expects exactly 2 parameters, 1 given in %s/array_some_all.php on line 16
NULL

Warning: array_all() expects exactly 2 parameters, 1 given in %s/array_some_all.php on line 17
NULL

*** Testing array_some() basic functionality ***
bool(false)
bool(true)
bool(true)
int(2)

*** Testing array_all() basic functionality ***
bool(true)
bool(false)
bool(false)
int(1)

*** Testing array_some() / array_all() edge cases ***
bool(true)
bool(false)

Done
