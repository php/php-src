--TEST--
Test array_pad() function : usage variations - unexpected values for 'input' argument 
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_pad() function by passing values to $input argument other than arrays
* and see that function outputs proper warning messages wherever expected.
* The $pad_size and $pad_value arguments passed are fixed values.
*/

echo "*** Testing array_pad() : passing non array values to \$input argument ***\n";

// Initialise $pad_size and $pad_value
$pad_size = 10;
$pad_value = 1;

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

       // string data
/*18*/ "string",
       'string',
       $heredoc,

       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behavior of array_pad()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --";
  var_dump( array_pad($input, $pad_size, $pad_value) );  // positive 'pad_size'
  var_dump( array_pad($input, -$pad_size, $pad_value) );  // negative 'pad_size'
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_pad() : passing non array values to $input argument ***

-- Iteration 1 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 2 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 3 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 4 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 5 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 6 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 7 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 8 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 9 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 10 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 11 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 12 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 13 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 14 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 15 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 16 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 17 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 18 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 19 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 20 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 21 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 22 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 23 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL

-- Iteration 24 --
Warning: array_pad(): The argument should be an array in %s on line %d
NULL

Warning: array_pad(): The argument should be an array in %s on line %d
NULL
Done
