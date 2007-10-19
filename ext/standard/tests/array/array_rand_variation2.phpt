--TEST--
Test array_rand() function : usage variations  - unexpected values for 'num_req' parameter
--FILE--
<?php
/* Prototype  : mixed array_rand(array input [, int num_req])
 * Description: Return key/keys for random entry/entries in the array 
 * Source code: ext/standard/array.c
*/

/*
* Test array_rand() with different types of values other than int passed to 'num_req' argument
* to see that function works with unexpeced data and generates warning message as required.
*/

echo "*** Testing array_rand() : unexpected values for 'num_req' parameter ***\n";

// Initialise function arguments
$input = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//define a class
class test
{
  var $t = 10;
  function __toString()
  {
    return "3object";
  }
}

//array of values to iterate over
$values = array(

        // int data
/*1*/   0,
        1,
        12345,
        -2345,

        // float data
/*5*/   10.5,
        -10.5,
        12.3456789000e10,
        12.3456789000E-10,
        .5,

        // null data
/*10*/  NULL,
        null,

        // boolean data
/*12*/  true,
        false,
        TRUE,
        FALSE,

        // empty data
/*16*/  "",
        '',

        // string data
/*18*/  "string",
        'string',

        // object data
/*20*/  new test(),

        // undefined data
/*21*/  @$undefined_var,

        // unset data
/*22*/  @$unset_var,
);


// loop through each element of the array for different values for 'num_req' argument
$count = 1;
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";
  var_dump( array_rand($input,$value) );  
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_rand() : unexpected values for 'num_req' parameter ***

-- Iteration 1 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 2 --
int(%d)

-- Iteration 3 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 4 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 5 --
array(10) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
}

-- Iteration 6 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 7 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 8 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 9 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 10 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 11 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 12 --
int(%d)

-- Iteration 13 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 14 --
int(%d)

-- Iteration 15 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 16 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 17 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 18 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 19 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 20 --

Notice: Object of class test could not be converted to int in %s on line %d
int(%d)

-- Iteration 21 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL

-- Iteration 22 --

Warning: array_rand(): Second argument has to be between 1 and the number of elements in the array in %s on line %d
NULL
Done
