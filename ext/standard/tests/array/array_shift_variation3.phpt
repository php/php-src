--TEST--
Test array_shift() function : usage variations - Pass array with different data types as keys
--FILE--
<?php
/*
 * Pass arrays with different data types as keys to test how array_shift() re-assigns keys
 */

echo "*** Testing array_shift() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed to $stack argument
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
/*4*/  'null uppercase' => array(
       NULL => 'null 1',
       ),

/*5*/  'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*6*/  'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

/*7*/  'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),

       // empty data
/*8*/  'empty double quotes' => array(
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

// loop through each element of $inputs to check the behavior of array_shift()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  var_dump( array_shift($input) );
  var_dump($input);
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_shift() : usage variations ***

-- Iteration 1 : int data --
string(4) "zero"
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(8) "positive"
  [2]=>
  string(8) "negative"
}

-- Iteration 2 : float data --
string(8) "positive"
array(2) {
  [0]=>
  string(8) "negative"
  [1]=>
  string(4) "half"
}

-- Iteration 3 : extreme floats data --
string(5) "large"
array(1) {
  [0]=>
  string(5) "small"
}

-- Iteration 4 : null uppercase data --
string(6) "null 1"
array(0) {
}

-- Iteration 5 : null lowercase data --
string(6) "null 2"
array(0) {
}

-- Iteration 6 : bool lowercase data --
string(6) "lowert"
array(1) {
  [0]=>
  string(6) "lowerf"
}

-- Iteration 7 : bool uppercase data --
string(6) "uppert"
array(1) {
  [0]=>
  string(6) "upperf"
}

-- Iteration 8 : empty double quotes data --
string(6) "emptyd"
array(0) {
}

-- Iteration 9 : empty single quotes data --
string(6) "emptys"
array(0) {
}

-- Iteration 10 : string data --
string(7) "stringd"
array(2) {
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
}

-- Iteration 11 : undefined data --
string(9) "undefined"
array(0) {
}

-- Iteration 12 : unset data --
string(5) "unset"
array(0) {
}
Done
