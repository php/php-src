--TEST--
Test array_values() function : usage variations - arrays of different data types
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of different data types as $input argument to array_values() to test behaviour
 */

echo "*** Testing array_values() : usage variations ***\n";

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

// arrays of different data types to be passed as $input
$inputs = array(

       // int data
/*1*/  'int' => array(
	   0,
       1,
       12345,
       -2345,
       ),

       // float data
/*2*/  'float' => array(
       10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,
       ),

       // null data
/*3*/ 'null' => array(
       NULL,
       null,
       ),

       // boolean data
/*4*/ 'bool' => array(
       true,
       false,
       TRUE,
       FALSE,
       ),
       
       // empty data
/*5*/ 'empty string' => array(
       "",
       '',
       ),
       
/*6*/ 'empty array' => array(
       ),

       // string data
/*7*/ 'string' => array(
       "string",
       'string',
       $heredoc,
       ),
       
       // object data
/*8*/ 'object' => array(
       new classA(),
       ),

       // undefined data
/*9*/ 'undefined' => array(
       @$undefined_var,
       ),

       // unset data
/*10*/ 'unset' => array(
       @$unset_var,
       ),

       // resource variable
/*11*/ 'resource' => array(
       $fp
       ),
);

// loop through each element of $inputs to check the behavior of array_values()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( array_values($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>

--EXPECTF--
*** Testing array_values() : usage variations ***

-- Iteration 1: int data --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(12345)
  [3]=>
  int(-2345)
}

-- Iteration 2: float data --
array(5) {
  [0]=>
  float(10.5)
  [1]=>
  float(-10.5)
  [2]=>
  float(123456789000)
  [3]=>
  float(1.23456789E-9)
  [4]=>
  float(0.5)
}

-- Iteration 3: null data --
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

-- Iteration 4: bool data --
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(false)
}

-- Iteration 5: empty string data --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}

-- Iteration 6: empty array data --
array(0) {
}

-- Iteration 7: string data --
array(3) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
  [2]=>
  string(11) "hello world"
}

-- Iteration 8: object data --
array(1) {
  [0]=>
  object(classA)#%d (0) {
  }
}

-- Iteration 9: undefined data --
array(1) {
  [0]=>
  NULL
}

-- Iteration 10: unset data --
array(1) {
  [0]=>
  NULL
}

-- Iteration 11: resource data --
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
Done