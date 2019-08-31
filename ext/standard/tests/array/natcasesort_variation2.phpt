--TEST--
Test natcasesort() function : usage variations - Pass arrays of different data types
--FILE--
<?php

/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of different data types to natcasesort() to test how they are sorted
 */

echo "*** Testing natcasesort() : usage variation ***\n";

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

// arrays of different data types to be passed to $array_arg argument
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
// loop through each element of $inputs to check the behavior of natcasesort()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump( natcasesort($input) );
	var_dump($input);
	$iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing natcasesort() : usage variation ***

-- Iteration 1 --
bool(true)
array(4) {
  [3]=>
  int(-2345)
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(12345)
}

-- Iteration 2 --
bool(true)
array(5) {
  [1]=>
  float(-10.5)
  [4]=>
  float(0.5)
  [3]=>
  float(1.23456789E-9)
  [0]=>
  float(10.5)
  [2]=>
  float(123456789000)
}

-- Iteration 3 --
bool(true)
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

-- Iteration 4 --
bool(true)
array(4) {
  [1]=>
  bool(false)
  [3]=>
  bool(false)
  [0]=>
  bool(true)
  [2]=>
  bool(true)
}

-- Iteration 5 --
bool(true)
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}

-- Iteration 6 --
bool(true)
array(0) {
}

-- Iteration 7 --
bool(true)
array(3) {
  [2]=>
  string(11) "hello world"
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
}

-- Iteration 8 --
bool(true)
array(1) {
  [0]=>
  object(classA)#%d (0) {
  }
}

-- Iteration 9 --
bool(true)
array(1) {
  [0]=>
  NULL
}

-- Iteration 10 --
bool(true)
array(1) {
  [0]=>
  NULL
}

-- Iteration 11 --
bool(true)
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
Done
