--TEST--
Test array_shift() function : usage variations - Pass arrays of different data types
--FILE--
<?php
/* Prototype  : mixed array_shift(array &$stack)
 * Description: Pops an element off the beginning of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays where values are of one data type to test behaviour of array_shift()
 */

echo "*** Testing array_shift() : usage variations ***\n";


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

// arrays of different data types to be passed to $stack argument
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

// loop through each element of $inputs to check the behavior of array_shift
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( array_shift($input) );
  var_dump($input);
  $iterator++;
};

fclose($fp);


echo "Done";
?>
--EXPECTF--
*** Testing array_shift() : usage variations ***

-- Iteration 1: int data --
int(0)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(12345)
  [2]=>
  int(-2345)
}

-- Iteration 2: float data --
float(10.5)
array(4) {
  [0]=>
  float(-10.5)
  [1]=>
  float(123456789000)
  [2]=>
  float(1.23456789E-9)
  [3]=>
  float(0.5)
}

-- Iteration 3: null data --
NULL
array(1) {
  [0]=>
  NULL
}

-- Iteration 4: bool data --
bool(true)
array(3) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
  [2]=>
  bool(false)
}

-- Iteration 5: empty string data --
string(0) ""
array(1) {
  [0]=>
  string(0) ""
}

-- Iteration 6: empty array data --
NULL
array(0) {
}

-- Iteration 7: string data --
string(6) "string"
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(11) "hello world"
}

-- Iteration 8: object data --
object(classA)#%d (0) {
}
array(0) {
}

-- Iteration 9: undefined data --
NULL
array(0) {
}

-- Iteration 10: unset data --
NULL
array(0) {
}

-- Iteration 11: resource data --
resource(%d) of type (stream)
array(0) {
}
Done