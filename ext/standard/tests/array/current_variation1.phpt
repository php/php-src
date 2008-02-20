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

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 2 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 3 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 4 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 5 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 6 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 7 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 8 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 9 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 10 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 11 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 12 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 13 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 14 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 15 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 16 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 17 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 18 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 19 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 20 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 21 --
NULL

-- Iteration 22 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 23 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 24 --

Warning: current(): Passed variable is not an array or object in %s on line %d
bool(false)
===DONE===
