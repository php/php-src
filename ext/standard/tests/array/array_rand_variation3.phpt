--TEST--
Test array_rand() function : usage variation - with MultiDimensional array 
--FILE--
<?php
/* Prototype  : mixed array_rand(array $input [, int $num_req])
 * Description: Return key/keys for random entry/entries in the array 
 * Source code: ext/standard/array.c
*/

/*
* Test behaviour of array_rand() function when multi-dimensional array 
* is passed to 'input' argument
*/

echo "*** Testing array_rand() : with multi-dimensional array ***\n";

// initialise the multi-dimensional array
$input = array(
       // array with int values
/*1*/  array(1, 2, 0, -0, -1, -2),

       // array with float values
       array(1.23, -1.23, 0.34, -0.34, 0e2, 2e-3, -2e2, -40e-2),             
               
       // array with single quoted strings 
/*3*/  array('one', '123numbers', 'hello\tworld', 'hello world\0', '12.34floatnum'),

       // array with double quoted strings 
       array("one","123numbers", "hello\tworld", "hello world\0", "12.34floatnum"),

       // array with bool values
/*5*/  array(true, TRUE, FALSE, false, TrUe, FaLsE),

       // array with hexa values
       array(0x123, -0x123, 0xabc, 0xABC, 0xab),
 
       // array with null values
/*7*/  array(null, NULL, "\0", Null, NuLl)

);

// initialise 'num_req' variable
$num_req = 3;

// calling array_rand() function with multi-dimensional array
var_dump( array_rand($input, $num_req) );

// looping to test array_rand() with each sub-array in the multi-dimensional array
echo "\n*** Testing array_rand() with arrays having different types of values ***\n";
$counter = 1;
foreach($input as $arr) {
  echo "\n-- Iteration $counter --\n";
  var_dump( array_rand($arr) );  // with default arguments
  var_dump( array_rand($arr, 3) );  // with default as well as optional arguments
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_rand() : with multi-dimensional array ***
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

*** Testing array_rand() with arrays having different types of values ***

-- Iteration 1 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 2 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 3 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 4 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 5 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 6 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 7 --
int(%d)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}
Done

