--TEST--
Test array_slice() function : usage variations - Pass different data types as $preserve_keys arg
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $preserve_keys argument to array_slice() to test behaviour
 */

echo "*** Testing array_slice() : usage variations ***\n";

// Initialise function arguments not being substituted
$input_array = array('one' => 1, 2, 99 => 3, 4);
$offset = 0;
$length = 3;

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

// unexpected values to be passed to $preserve_keys argument
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
);

// loop through each element of $inputs to check the behavior of array_slice()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_slice($input_array, $offset, $length, $input) );
  $iterator++;
};

echo "Done";
?>

--EXPECTF--
*** Testing array_slice() : usage variations ***

-- Iteration 1 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 2 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 3 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 4 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 5 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 6 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 7 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 8 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 9 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 10 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 11 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 12 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 13 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 14 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 15 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 16 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 17 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 18 --

Warning: array_slice() expects parameter 4 to be boolean, array given in %s on line %d
NULL

-- Iteration 19 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 20 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 21 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [99]=>
  int(3)
}

-- Iteration 22 --

Warning: array_slice() expects parameter 4 to be boolean, object given in %s on line %d
NULL

-- Iteration 23 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- Iteration 24 --
array(3) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
  [1]=>
  int(3)
}
Done