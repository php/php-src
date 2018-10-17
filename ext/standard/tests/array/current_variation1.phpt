--TEST--
Test current() function : usage variations - Pass different data types as $array_arg arg
--FILE--
<?php
/* Prototype  : mixed current(array $array_arg)
 * Description: Return the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 * Alias to functions: pos
 */

/*
 * Pass different data types as $array_arg argument to current() to test behaviour
 */

echo "*** Testing current() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
	var $var1;
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

// loop through each element of $inputs to check the behavior of current()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( current($input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing current() : usage variations ***

-- Iteration 1 --

Warning: current() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 2 --

Warning: current() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 3 --

Warning: current() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 4 --

Warning: current() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 5 --

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: current() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: current() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 13 --

Warning: current() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 14 --

Warning: current() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 15 --

Warning: current() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 16 --

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: current() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 21 --
NULL

-- Iteration 22 --

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 23 --

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: current() expects parameter 1 to be array, resource given in %s on line %d
NULL
===DONE===
