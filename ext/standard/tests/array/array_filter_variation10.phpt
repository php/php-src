--TEST--
Test array_filter() function : usage variations - using the array keys inside 'callback'
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback [, bool $use_type = ARRAY_FILTER_USE_VALUE]])
 * Description: Filters elements from the array via the callback.
 * Source code: ext/standard/array.c
*/

/*
* Using array keys as an argument to the 'callback'
*/

echo "*** Testing array_filter() : usage variations - using array keys in 'callback' ***\n";

$input = array(0, 1, -1, 10, 100, 1000, 'Hello', null);
$small = array(123);

function dump($value, $key)
{
  echo "$key = $value\n";
}

var_dump( array_filter($input, 'dump', true) );

echo "*** Testing array_filter() : usage variations - 'callback' filters based on key value ***\n";

function dump2($value, $key)
{
  return $key > 4;
}

var_dump( array_filter($input, 'dump2', true) );

echo "*** Testing array_filter() : usage variations - 'callback' expecting second argument ***\n";

try {
	var_dump( array_filter($small, 'dump', false) );
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

echo "*** Testing array_filter() with various use types ***\n";

$mixed = array(1 => 'a', 2 => 'b', 'a' => 1, 'b' => 2);

var_dump(array_filter($mixed, 'is_numeric', ARRAY_FILTER_USE_KEY));

var_dump(array_filter($mixed, 'is_numeric', 0));

var_dump(array_filter($mixed, 'is_numeric', ARRAY_FILTER_USE_BOTH));

echo "Done"
?>
--EXPECTF--
*** Testing array_filter() : usage variations - using array keys in 'callback' ***
0 = 0
1 = 1
2 = -1
3 = 10
4 = 100
5 = 1000
6 = Hello
7 = 
array(0) {
}
*** Testing array_filter() : usage variations - 'callback' filters based on key value ***
array(3) {
  [5]=>
  int(1000)
  [6]=>
  string(5) "Hello"
  [7]=>
  NULL
}
*** Testing array_filter() : usage variations - 'callback' expecting second argument ***
Exception: Too few arguments to function dump(), 1 passed and exactly 2 expected
*** Testing array_filter() with various use types ***
array(2) {
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "b"
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}

Warning: is_numeric() expects exactly 1 parameter, 2 given in %s on line 48

Warning: is_numeric() expects exactly 1 parameter, 2 given in %s on line 48

Warning: is_numeric() expects exactly 1 parameter, 2 given in %s on line 48

Warning: is_numeric() expects exactly 1 parameter, 2 given in %s on line 48
array(0) {
}
Done
