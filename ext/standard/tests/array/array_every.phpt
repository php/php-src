--TEST--
Test array_every() function
--FILE--
<?php
/* 
	Prototype: bool array_every(array $array, mixed $callbac);
	Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($item)
{
	return is_int($item);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

try {
    var_dump(array_every());
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    var_dump(array_every(true));
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    var_dump(array_every([]));
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "\n*** Testing basic functionality ***\n";

var_dump(array_every(array(1, 2, 3), 'is_int_ex'));
var_dump(array_every(array('hello', 1, 2, 3), 'is_int_ex'));
$iterations = 0;
var_dump(array_every(array('hello', 1, 2, 3), function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
}));
var_dump($iterations);

echo "\n*** Testing second argument to predicate ***\n";

var_dump(array_every([1, 2, 3], function($item, $key) {
	var_dump($key);
	return true;
}));

echo "\n*** Testing third argument to predicate ***\n";

var_dump(array_every([1, 2, 3], function($item, $key, $array) {
	var_dump($array);
	return true;
}));

echo "\n*** Testing edge cases ***\n";

var_dump(array_every(array(), 'is_int_ex'));

echo "\nDone";
?> 
--EXPECTF--
*** Testing not enough or wrong arguments ***
Exception: array_every() expects exactly 2 parameters, 0 given
Exception: array_every() expects exactly 2 parameters, 1 given
Exception: array_every() expects exactly 2 parameters, 1 given

*** Testing basic functionality ***
bool(true)
bool(false)
bool(false)
int(1)

*** Testing second argument to predicate ***
int(0)
int(1)
int(2)
bool(true)

*** Testing third argument to predicate ***
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)

*** Testing edge cases ***
bool(true)

Done
