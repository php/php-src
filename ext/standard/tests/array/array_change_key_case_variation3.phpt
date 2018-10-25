--TEST--
Test array_change_key_case() function : usage variations - different data types as keys
--FILE--
<?php
/* Prototype  : array array_change_key_case(array $input [, int $case])
 * Description: Returns an array with all string keys lowercased [or uppercased]
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays with different data types as keys to array_change_key_case()
 * to test conversion
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays of different data types to be passed to $input argument
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

       'extreme floats' => array(
       12.3456789000e6 => 'large',
       12.3456789000E-10 => 'small',
       ),

       // null data
/*3*/ 'null uppercase' => array(
       NULL => 'null 1',
       ),
       'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*4*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),
       'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),

       // empty data
/*5*/ 'empty double quotes' => array(
       "" => 'emptyd',
       ),
       'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*6*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // undefined data
/*8*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*9*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each sub-array of $inputs to check the behavior of array_change_key_case()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  var_dump( array_change_key_case($input, CASE_UPPER) );
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_change_key_case() : usage variations ***

-- Iteration 1 : int data --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [-2345]=>
  string(8) "negative"
}

-- Iteration 2 : float data --
array(3) {
  [10]=>
  string(8) "positive"
  [-10]=>
  string(8) "negative"
  [0]=>
  string(4) "half"
}

-- Iteration 3 : extreme floats data --
array(2) {
  [12345678]=>
  string(5) "large"
  [0]=>
  string(5) "small"
}

-- Iteration 4 : null uppercase data --
array(1) {
  [""]=>
  string(6) "null 1"
}

-- Iteration 5 : null lowercase data --
array(1) {
  [""]=>
  string(6) "null 2"
}

-- Iteration 6 : bool lowercase data --
array(2) {
  [1]=>
  string(6) "lowert"
  [0]=>
  string(6) "lowerf"
}

-- Iteration 7 : bool uppercase data --
array(2) {
  [1]=>
  string(6) "uppert"
  [0]=>
  string(6) "upperf"
}

-- Iteration 8 : empty double quotes data --
array(1) {
  [""]=>
  string(6) "emptyd"
}

-- Iteration 9 : empty single quotes data --
array(1) {
  [""]=>
  string(6) "emptys"
}

-- Iteration 10 : string data --
array(3) {
  ["STRINGD"]=>
  string(7) "stringd"
  ["STRINGS"]=>
  string(7) "strings"
  ["HELLO WORLD"]=>
  string(7) "stringh"
}

-- Iteration 11 : undefined data --
array(1) {
  [""]=>
  string(9) "undefined"
}

-- Iteration 12 : unset data --
array(1) {
  [""]=>
  string(5) "unset"
}
Done
