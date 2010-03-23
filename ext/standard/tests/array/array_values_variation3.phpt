--TEST--
Test array_values() function : usage variations - array keys different data types
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays where the keys are different data types as $input argument
 * to array_values() to test behaviour
 */

echo "*** Testing array_values() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed as $input
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // float data
/*2*/  'float' => array(
       10.5 => 'positive',
       -10.5 => 'negative',
       .5 => 'half',
       ),
       
/*3*/  'extreme floats' => array(
       12.3456789000e10 => 'large',
       12.3456789000E-10 => 'small',
       ),

       // null data
/*4*/ 'null uppercase' => array(
       NULL => 'null 1',
       ), 
       
/*5*/  'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*6*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),
       
/*7*/  'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),
       
       // empty data
/*8*/ 'empty double quotes' => array(
       "" => 'emptyd',
       ),
       
/*9*/  'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*10*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // undefined data
/*11*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*12*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each element of $inputs to check the behavior of array_values()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( array_values($input) );
  $iterator++;
};
echo "Done";
?>

--EXPECTF--
*** Testing array_values() : usage variations ***

-- Iteration 1: int data --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
}

-- Iteration 2: float data --
array(3) {
  [0]=>
  string(8) "positive"
  [1]=>
  string(8) "negative"
  [2]=>
  string(4) "half"
}

-- Iteration 3: extreme floats data --
array(2) {
  [0]=>
  string(5) "large"
  [1]=>
  string(5) "small"
}

-- Iteration 4: null uppercase data --
array(1) {
  [0]=>
  string(6) "null 1"
}

-- Iteration 5: null lowercase data --
array(1) {
  [0]=>
  string(6) "null 2"
}

-- Iteration 6: bool lowercase data --
array(2) {
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
}

-- Iteration 7: bool uppercase data --
array(2) {
  [0]=>
  string(6) "uppert"
  [1]=>
  string(6) "upperf"
}

-- Iteration 8: empty double quotes data --
array(1) {
  [0]=>
  string(6) "emptyd"
}

-- Iteration 9: empty single quotes data --
array(1) {
  [0]=>
  string(6) "emptys"
}

-- Iteration 10: string data --
array(3) {
  [0]=>
  string(7) "stringd"
  [1]=>
  string(7) "strings"
  [2]=>
  string(7) "stringh"
}

-- Iteration 11: undefined data --
array(1) {
  [0]=>
  string(9) "undefined"
}

-- Iteration 12: unset data --
array(1) {
  [0]=>
  string(5) "unset"
}
Done