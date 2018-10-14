--TEST--
Test usort() function : usage variations - Pass different data types as $array_arg arg
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $array_arg argument to usort() to test behaviour
 */

echo "*** Testing usort() : usage variations ***\n";

// Initialise function arguments not being substituted
function cmp_function($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

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

// unexpected values to be passed to $array_arg argument
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

// loop through each element of $inputs to check the behavior of usort()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( usort($input, 'cmp_function') );
  $iterator++;
};

//closing resource
fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing usort() : usage variations ***

-- Iteration 1 --

Warning: usort() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 2 --

Warning: usort() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 3 --

Warning: usort() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 4 --

Warning: usort() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 5 --

Warning: usort() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: usort() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: usort() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: usort() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: usort() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: usort() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: usort() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: usort() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 13 --

Warning: usort() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 14 --

Warning: usort() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 15 --

Warning: usort() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 16 --

Warning: usort() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: usort() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 18 --
bool(true)

-- Iteration 19 --

Warning: usort() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: usort() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: usort() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: usort() expects parameter 1 to be array, object given in %s on line %d
NULL

-- Iteration 23 --

Warning: usort() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: usort() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 25 --

Warning: usort() expects parameter 1 to be array, resource given in %s on line %d
NULL
===DONE===
