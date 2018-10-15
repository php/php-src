--TEST--
Test array_key_exists() function : usage variations - Pass different data types to $search arg
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Pass different data types as $search argument to array_key_exists() to test behaviour
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

// Initialise function arguments not being substituted
$key = 'val';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $search argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "string",
       'string',
       $heredoc,

       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of array_key_exists()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_key_exists($key, $input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- Iteration 1 --

Warning: array_key_exists() expects parameter 2 to be array, int given in %s on line %d
NULL

-- Iteration 2 --

Warning: array_key_exists() expects parameter 2 to be array, int given in %s on line %d
NULL

-- Iteration 3 --

Warning: array_key_exists() expects parameter 2 to be array, int given in %s on line %d
NULL

-- Iteration 4 --

Warning: array_key_exists() expects parameter 2 to be array, int given in %s on line %d
NULL

-- Iteration 5 --

Warning: array_key_exists() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: array_key_exists() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: array_key_exists() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: array_key_exists() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: array_key_exists() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: array_key_exists() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: array_key_exists() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: array_key_exists() expects parameter 2 to be array, bool given in %s on line %d
NULL

-- Iteration 13 --

Warning: array_key_exists() expects parameter 2 to be array, bool given in %s on line %d
NULL

-- Iteration 14 --

Warning: array_key_exists() expects parameter 2 to be array, bool given in %s on line %d
NULL

-- Iteration 15 --

Warning: array_key_exists() expects parameter 2 to be array, bool given in %s on line %d
NULL

-- Iteration 16 --

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 18 --
bool(false)

-- Iteration 19 --

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 22 --
bool(false)

-- Iteration 23 --

Warning: array_key_exists() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: array_key_exists() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 25 --

Warning: array_key_exists() expects parameter 2 to be array, resource given in %s on line %d
NULL
Done
