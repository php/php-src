--TEST--
Test array_slice() function : usage variations - Pass different data types as $length arg
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $length argument to array_slice to test behaviour
 */

echo "*** Testing array_slice() : usage variations ***\n";

// Initialise function arguments not being substituted
$input_array = array('one' => 1, 2, 'three' => 3, 4);
$offset = 2;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed to $length argument
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

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,
);

// loop through each element of $inputs to check the behavior of array_slice
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_slice($input_array, $offset, $input) );
  $iterator++;
};

echo "Done";
?>

--EXPECTF--
*** Testing array_slice() : usage variations ***

-- Iteration 1 --
array(0) {
}

-- Iteration 2 --
array(1) {
  ["three"]=>
  int(3)
}

-- Iteration 3 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 4 --
array(0) {
}

-- Iteration 5 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 6 --
array(0) {
}

-- Iteration 7 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 8 --
array(0) {
}

-- Iteration 9 --
array(0) {
}

-- Iteration 10 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 11 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 12 --
array(1) {
  ["three"]=>
  int(3)
}

-- Iteration 13 --
array(0) {
}

-- Iteration 14 --
array(1) {
  ["three"]=>
  int(3)
}

-- Iteration 15 --
array(0) {
}

-- Iteration 16 --
array(0) {
}

-- Iteration 17 --
array(0) {
}

-- Iteration 18 --
array(0) {
}

-- Iteration 19 --
array(0) {
}

-- Iteration 20 --
array(0) {
}

-- Iteration 21 --
array(0) {
}

-- Iteration 22 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}

-- Iteration 23 --
array(2) {
  ["three"]=>
  int(3)
  [0]=>
  int(4)
}
Done
