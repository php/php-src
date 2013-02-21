--TEST--
Test array_filter() function : usage variations - using the array keys inside 'callback'
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback [, bool $use_keys = false]])
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

var_dump( array_filter($small, 'dump', false) );

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

Warning: Missing argument 2 for dump() in /home/tjerk/work/php/php-src/ext/standard/tests/array/array_filter_variation10.php on line %d

Notice: Undefined variable: key in /home/tjerk/work/php/php-src/ext/standard/tests/array/array_filter_variation10.php on line %d
 = 123
array(0) {
}
Done
