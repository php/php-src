--TEST--
Test array_merge() function : usage variations - Pass different data types as $arr2 arg
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $arr2 argument to array_merge() to test behaviour
 */

echo "*** Testing array_merge() : usage variations ***\n";

// Initialise function arguments not being substituted
$arr1 = array (1, 2);

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

// unexpected values to be passed to $arr2 argument
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

// loop through each element of $inputs to check the behavior of array_merge()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_merge($arr1, $input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***

-- Iteration 1 --

Warning: array_merge(): Expected parameter 2 to be an array, int given in %s on line %d
NULL

-- Iteration 2 --

Warning: array_merge(): Expected parameter 2 to be an array, int given in %s on line %d
NULL

-- Iteration 3 --

Warning: array_merge(): Expected parameter 2 to be an array, int given in %s on line %d
NULL

-- Iteration 4 --

Warning: array_merge(): Expected parameter 2 to be an array, int given in %s on line %d
NULL

-- Iteration 5 --

Warning: array_merge(): Expected parameter 2 to be an array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: array_merge(): Expected parameter 2 to be an array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: array_merge(): Expected parameter 2 to be an array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: array_merge(): Expected parameter 2 to be an array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: array_merge(): Expected parameter 2 to be an array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: array_merge(): Expected parameter 2 to be an array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: array_merge(): Expected parameter 2 to be an array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: array_merge(): Expected parameter 2 to be an array, bool given in %s on line %d
NULL

-- Iteration 13 --

Warning: array_merge(): Expected parameter 2 to be an array, bool given in %s on line %d
NULL

-- Iteration 14 --

Warning: array_merge(): Expected parameter 2 to be an array, bool given in %s on line %d
NULL

-- Iteration 15 --

Warning: array_merge(): Expected parameter 2 to be an array, bool given in %s on line %d
NULL

-- Iteration 16 --

Warning: array_merge(): Expected parameter 2 to be an array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: array_merge(): Expected parameter 2 to be an array, string given in %s on line %d
NULL

-- Iteration 18 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

-- Iteration 19 --

Warning: array_merge(): Expected parameter 2 to be an array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: array_merge(): Expected parameter 2 to be an array, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: array_merge(): Expected parameter 2 to be an array, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: array_merge(): Expected parameter 2 to be an array, object given in %s on line %d
NULL

-- Iteration 23 --

Warning: array_merge(): Expected parameter 2 to be an array, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: array_merge(): Expected parameter 2 to be an array, null given in %s on line %d
NULL

-- Iteration 25 --

Warning: array_merge(): Expected parameter 2 to be an array, resource given in %s on line %d
NULL
Done
