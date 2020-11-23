--TEST--
Test array_diff_assoc() function : variation - array containing different data types
--FILE--
<?php
/*
 * Test how array_diff_assoc() compares indexed arrays containing different data types
 */

echo "\n*** Testing array_diff_assoc() : usage variations ***\n";

$array = array(1, 2, 3);

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

//array of different data types to be passed to $arr1 argument
$inputs = array(

       // int data
/*1*/
'int' => array(
       0,
       1,
       12345,
       -2345),

       // float data
/*2*/
'float' => array(
       10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5),

       // null data
/*3*/
'null' => array(
       NULL,
       null),

       // boolean data
/*4*/
'bool' => array(
       true,
       false,
       TRUE,
       FALSE),

       // empty data
/*5*/
'empty' => array(
       "",
       ''),

       // string data
/*6*/
'string' => array(
       "string",
       'string',
       $heredoc),

       // binary data
/*7*/
'binary' => array(
       b"binary",
       (binary)"binary"),

       // object data
/*8*/
'object' => array(
      new classA()),

       // undefined data
/*9*/
'undefined' => array(
       @$undefined_var),

       // unset data
/*10*/
'unset' => array(
      @$unset_var),
);

// loop through each element of $inputs to check the behavior of array_diff_assoc
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_diff_assoc($input, $array));
  $iterator++;
};
echo "Done";
?>
--EXPECTF--
*** Testing array_diff_assoc() : usage variations ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 3 --
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

-- Iteration 4 --
array(3) {
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(false)
}

-- Iteration 5 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}

-- Iteration 6 --
array(3) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
  [2]=>
  string(11) "hello world"
}

-- Iteration 7 --
array(2) {
  [0]=>
  string(6) "binary"
  [1]=>
  string(6) "binary"
}

-- Iteration 8 --
array(1) {
  [0]=>
  object(classA)#%d (0) {
  }
}

-- Iteration 9 --
array(1) {
  [0]=>
  NULL
}

-- Iteration 10 --
array(1) {
  [0]=>
  NULL
}
Done
