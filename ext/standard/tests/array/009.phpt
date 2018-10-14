--TEST--
Test key(), current(), next() & reset() functions
--FILE--
<?php
/* Prototype & Usage:
   mixed key ( array &$array ) -> returns the index element of the current array position
   mixed current ( array &$array ) -> returns the current element in the array
   mixed next ( array &$array ) -> similar to current() but advances the internal pointer to next element
   mixed reset ( array &$array ) -> Reset the internal pointer to first element
*/

$basic_arrays = array (
  array(0),  // array with element as 0
  array(1),  // array with single element
  array(1,2, 3, -1, -2, -3),               // array of integers
  array(1.1, 2.2, 3.3, -1.1, -2.2, -3.3),  // array of floats
  array('a', 'b', 'c', "ab", "ac", "ad"),  // string array
  array("a" => "apple", "b" => "book", "c" => "cook"),  // associative array
  array('d' => 'drink', 'p' => 'port', 's' => 'set'),   // another associative array
  array(1 => 'One', 2 => 'two', 3 => "three")           // associative array with key as integers
);

$varient_arrays = array (
   array(),    // empty array
   array(""),  // array with null string
   array(NULL),// array with NULL
   array(null),// array with null
   array(NULL, true, null, "", 1), // mixed array
   array(-1.5 => "test", -2 => "rest", 2.5 => "two",
         "" => "string", 0 => "zero", "" => "" ) // mixed array
);

echo "*** Testing basic operations ***\n";
$loop_count = 1;
foreach ($basic_arrays as $sub_array )  {
  echo "-- Iteration $loop_count --\n";
  $loop_count++;
  $c = count ($sub_array);
  $c++; // increment by one to create the situation of accessing beyond array size
  while ( $c ) {
    var_dump( current($sub_array)); // current element
    var_dump( key($sub_array) );    // key of the current element
    var_dump( next($sub_array) );   // move to next element
    $c --;
  }
  var_dump( reset($sub_array) );    // reset the internal pointer to first element
  var_dump( key($sub_array) );      // access the array after reset
  var_dump( $sub_array );           // dump the array to see that its intact

  echo "\n";
}

echo "\n*** Testing possible variations ***\n";
$loop_count = 1;
foreach ($varient_arrays as $sub_array )  {
  echo "-- Iteration $loop_count --\n";
  $loop_count++;
  $c = count ($sub_array);
  $c++; // increment by one to create the situation of accessing beyond array size
  while ( $c ) {
    var_dump( current($sub_array)); // current element
    var_dump( key($sub_array) );    // key of the current element
    var_dump( next($sub_array) );   // move to next element
    $c --;
  }
  var_dump( reset($sub_array) );    // reset the internal pointer to first element
  var_dump( key($sub_array) );      // access the array after reset
  var_dump( $sub_array );           // dump the array to see that its intact
  echo "\n";
}

/*test these functions on array which is already unset */
echo "\n-- Testing variation: when array is unset --\n";
$unset_array = array (1);
unset($unset_array);

var_dump( current($unset_array) );
var_dump( key($unset_array) );
var_dump( next($unset_array) );
var_dump( reset($unset_array) );


echo "\n*** Testing error conditions ***\n";
//Zero argument, expected 1 argument
var_dump( key() );
var_dump( current() );
var_dump( reset() );
var_dump( next() );

// args more than expected, expected 1 argument
$temp_array = array(1);
var_dump( key($temp_array, $temp_array) );
var_dump( current($temp_array, $temp_array) );
var_dump( reset($temp_array, $temp_array) );
var_dump( next($temp_array, $temp_array) );

// invalid args type, valid argument: array
$int_var = 1;
$float_var = 1.5;
$string = "string";
var_dump( key($int_var) );
var_dump( key($float_var) );
var_dump( key($string) );

var_dump( current($int_var) );
var_dump( current($float_var) );
var_dump( current($string) );

var_dump( next($int_var) );
var_dump( next($float_var) );
var_dump( next($string) );

var_dump( reset($int_var) );
var_dump( reset($float_var) );
var_dump( reset($string) );

echo "Done\n";
?>
--EXPECTF--
*** Testing basic operations ***
-- Iteration 1 --
int(0)
int(0)
bool(false)
bool(false)
NULL
bool(false)
int(0)
int(0)
array(1) {
  [0]=>
  int(0)
}

-- Iteration 2 --
int(1)
int(0)
bool(false)
bool(false)
NULL
bool(false)
int(1)
int(0)
array(1) {
  [0]=>
  int(1)
}

-- Iteration 3 --
int(1)
int(0)
int(2)
int(2)
int(1)
int(3)
int(3)
int(2)
int(-1)
int(-1)
int(3)
int(-2)
int(-2)
int(4)
int(-3)
int(-3)
int(5)
bool(false)
bool(false)
NULL
bool(false)
int(1)
int(0)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(-1)
  [4]=>
  int(-2)
  [5]=>
  int(-3)
}

-- Iteration 4 --
float(1.1)
int(0)
float(2.2)
float(2.2)
int(1)
float(3.3)
float(3.3)
int(2)
float(-1.1)
float(-1.1)
int(3)
float(-2.2)
float(-2.2)
int(4)
float(-3.3)
float(-3.3)
int(5)
bool(false)
bool(false)
NULL
bool(false)
float(1.1)
int(0)
array(6) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
  [2]=>
  float(3.3)
  [3]=>
  float(-1.1)
  [4]=>
  float(-2.2)
  [5]=>
  float(-3.3)
}

-- Iteration 5 --
string(1) "a"
int(0)
string(1) "b"
string(1) "b"
int(1)
string(1) "c"
string(1) "c"
int(2)
string(2) "ab"
string(2) "ab"
int(3)
string(2) "ac"
string(2) "ac"
int(4)
string(2) "ad"
string(2) "ad"
int(5)
bool(false)
bool(false)
NULL
bool(false)
string(1) "a"
int(0)
array(6) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(2) "ab"
  [4]=>
  string(2) "ac"
  [5]=>
  string(2) "ad"
}

-- Iteration 6 --
string(5) "apple"
string(1) "a"
string(4) "book"
string(4) "book"
string(1) "b"
string(4) "cook"
string(4) "cook"
string(1) "c"
bool(false)
bool(false)
NULL
bool(false)
string(5) "apple"
string(1) "a"
array(3) {
  ["a"]=>
  string(5) "apple"
  ["b"]=>
  string(4) "book"
  ["c"]=>
  string(4) "cook"
}

-- Iteration 7 --
string(5) "drink"
string(1) "d"
string(4) "port"
string(4) "port"
string(1) "p"
string(3) "set"
string(3) "set"
string(1) "s"
bool(false)
bool(false)
NULL
bool(false)
string(5) "drink"
string(1) "d"
array(3) {
  ["d"]=>
  string(5) "drink"
  ["p"]=>
  string(4) "port"
  ["s"]=>
  string(3) "set"
}

-- Iteration 8 --
string(3) "One"
int(1)
string(3) "two"
string(3) "two"
int(2)
string(5) "three"
string(5) "three"
int(3)
bool(false)
bool(false)
NULL
bool(false)
string(3) "One"
int(1)
array(3) {
  [1]=>
  string(3) "One"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
}


*** Testing possible variations ***
-- Iteration 1 --
bool(false)
NULL
bool(false)
bool(false)
NULL
array(0) {
}

-- Iteration 2 --
string(0) ""
int(0)
bool(false)
bool(false)
NULL
bool(false)
string(0) ""
int(0)
array(1) {
  [0]=>
  string(0) ""
}

-- Iteration 3 --
NULL
int(0)
bool(false)
bool(false)
NULL
bool(false)
NULL
int(0)
array(1) {
  [0]=>
  NULL
}

-- Iteration 4 --
NULL
int(0)
bool(false)
bool(false)
NULL
bool(false)
NULL
int(0)
array(1) {
  [0]=>
  NULL
}

-- Iteration 5 --
NULL
int(0)
bool(true)
bool(true)
int(1)
NULL
NULL
int(2)
string(0) ""
string(0) ""
int(3)
int(1)
int(1)
int(4)
bool(false)
bool(false)
NULL
bool(false)
NULL
int(0)
array(5) {
  [0]=>
  NULL
  [1]=>
  bool(true)
  [2]=>
  NULL
  [3]=>
  string(0) ""
  [4]=>
  int(1)
}

-- Iteration 6 --
string(4) "test"
int(-1)
string(4) "rest"
string(4) "rest"
int(-2)
string(3) "two"
string(3) "two"
int(2)
string(0) ""
string(0) ""
string(0) ""
string(4) "zero"
string(4) "zero"
int(0)
bool(false)
bool(false)
NULL
bool(false)
string(4) "test"
int(-1)
array(5) {
  [-1]=>
  string(4) "test"
  [-2]=>
  string(4) "rest"
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
  [0]=>
  string(4) "zero"
}


-- Testing variation: when array is unset --

Notice: Undefined variable: unset_array in %s on line %d

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

Notice: Undefined variable: unset_array in %s on line %d

Warning: key() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: next() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: reset() expects parameter 1 to be array, null given in %s on line %d
NULL

*** Testing error conditions ***

Warning: key() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: current() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: reset() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: next() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: key() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: current() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: reset() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: next() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: key() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: key() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: key() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: current() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: next() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: next() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: next() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: reset() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: reset() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: reset() expects parameter 1 to be array, string given in %s on line %d
NULL
Done
