--TEST--
Test array_values() function : usage variations - Pass different data types as $input arg
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $input argument to array_values() to test behaviour
 */

echo "*** Testing array_values() : usage variations ***\n";

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

// unexpected values to be passed to $input argument
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

// loop through each element of $inputs to check the behavior of array_values()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_values($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>

--EXPECTF--
*** Testing array_values() : usage variations ***

-- Iteration 1 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 2 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 3 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 4 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 5 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 6 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 7 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 8 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 9 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 10 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 11 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 12 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 13 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 14 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 15 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 16 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 17 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 18 --
array(0) {
}

-- Iteration 19 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 20 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 21 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 22 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 23 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 24 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL

-- Iteration 25 --

Warning: array_values(): The argument should be an array in %s on line %d
NULL
Done