--TEST--
Test array_fill() function : usage variations  - unexpected values for 'num' argument 
--FILE--
<?php
/* Prototype  : proto array array_fill(int start_key, int num, mixed val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val 
 * Source code: ext/standard/array.c
 */

/*
 * testing array_fill() by passing different unexpected values for 'num' argument  
 */

echo "*** Testing array_fill() : usage variations ***\n";

// Initialise function arguments not being substituted 
$start_key = 0;
$val = 100;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//define a class
class test
{
  var $t = 10;
  function __toString()
  {
    return "testObject";
  }
}


//array of different  values for 'num' argument
$values = array(

            // float values
  /* 1  */  2.5,
            -2.5,
            0.5e1,
            0.5E-1,
            .5,

            // array values
  /* 6  */  array(),
            array(0),
            array(1),
            array(1, 2),
            array('color' => 'red', 'item' => 'pen'),

            // null values
  /* 11 */  NULL,
            null,

            // boolean values
  /* 13 */  true,
            false,
            TRUE,
            FALSE,

            // empty string
  /* 17 */  "",
            '',

            // string values
  /* 19 */  "string",
            'string',

            // objects
  /* 21 */  new test(),

            // undefined  variable
            @$undefined_var,

            // unset variable
  /* 24 */  @$unset_var,

);

// loop through each element of the array for num
// check the working of array_fill
echo "--- Testing array_fill() with different values for 'num' arg ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++)
{
  echo "-- Iteration $counter --\n";
  $num = $values[$index];

  var_dump( array_fill($start_key,$num,$val) );
 
  $counter ++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_fill() : usage variations ***
--- Testing array_fill() with different values for 'num' arg ---
-- Iteration 1 --
array(2) {
  [0]=>
  int(100)
  [1]=>
  int(100)
}
-- Iteration 2 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 3 --
array(5) {
  [0]=>
  int(100)
  [1]=>
  int(100)
  [2]=>
  int(100)
  [3]=>
  int(100)
  [4]=>
  int(100)
}
-- Iteration 4 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 5 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 6 --

Warning: array_fill() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 7 --

Warning: array_fill() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 8 --

Warning: array_fill() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 9 --

Warning: array_fill() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: array_fill() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 12 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 13 --
array(1) {
  [0]=>
  int(100)
}
-- Iteration 14 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 15 --
array(1) {
  [0]=>
  int(100)
}
-- Iteration 16 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 17 --

Warning: array_fill() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 18 --

Warning: array_fill() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 19 --

Warning: array_fill() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: array_fill() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 21 --

Warning: array_fill() expects parameter 2 to be long, object given in %s on line %d
NULL
-- Iteration 22 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
-- Iteration 23 --

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
Done
