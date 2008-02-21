--TEST--
Test next() function : usage variation - Pass different data types as $array_arg
--FILE--
<?php
/* Prototype  : mixed next(array $array_arg)
 * Description: Move array argument's internal pointer to the next element and return it 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $array_arg argument to next() to test behaviour
 */

echo "*** Testing next() : variation ***\n";

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

// loop through each element of $inputs to check the behavior of next()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( next($input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing next() : variation ***

-- Iteration 1 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 2 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 3 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 4 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 5 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 6 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 7 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 8 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 9 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 10 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 11 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 12 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 13 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 14 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 15 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 16 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 17 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 18 --
bool(false)

-- Iteration 19 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 20 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 21 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 22 --
bool(false)

-- Iteration 23 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 24 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)

-- Iteration 25 --

Warning: next(): Passed variable is not an array or object in %s on line %d
bool(false)
===DONE===
