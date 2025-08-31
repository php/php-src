--TEST--
Test array_intersect_assoc() function : usage variations - assoc array with diff values for 'arr1' argument
--FILE--
<?php
/*
 * Testing the functionality of array_intersect_assoc() by passing different
 * associative arrays having different possible values to $arr1 argument.
 * The $arr2 argument passed is a fixed array
*/

echo "*** Testing array_intersect_assoc() : assoc array with diff values to \$arr1 argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// get a class
class classA
{
  public function __toString(){
    return "Class A object";
  }
}

// get a heredoc string
$heredoc = <<<EOT
Hello world
EOT;

// different variations of associative arrays to be passed to $arr1 argument
$arrays = array (

       // empty array
/*1*/  array(),

       // arrays with integer values
       array('0' => 0),
       array("1" => 1),
       array("one" => 1, 'two' => 2, "three" => 3, 4 => 4),

       // arrays with float values
/*5*/  array("float" => 2.3333),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => 33333333.333),

       // arrays with string values
/*7*/  array(111 => "\tHello", "red" => "col\tor", 2 => "\v\fworld", 3 =>  "pen\n"),
       array(111 => '\tHello', "red" => 'col\tor', 2 => '\v\fworld', 3 =>  'pen\n'),
       array(1 => "hello", "heredoc" => $heredoc),

       // array with object, unset variable and resource variable
/*10*/ array(11 => new classA(), "unset" => @$unset_var, "resource" => $fp),

       // array with mixed values
/*11*/ array(1 => 'hello', 2 => new classA(), 222 => "fruit",
             'resource' => $fp, "int" => 133, "float" => 444.432,
             "unset" => @$unset_var, "heredoc" => $heredoc)
);

// array to be passsed to $arr2 argument
$arr2 = array(0 => "0", 1, "two" => 2, "float" => 2.3333, "f1" => 1.2,
              "f4" => 33333333.333, 111 => "\tHello", 3 => 'pen\n', '\v\fworld',
              "heredoc" => "Hello world", 11 => new classA(), "resource" => $fp,
              "int" => 133, 222 => "fruit");

// loop through each sub-array within $arrays to check the behavior of array_intersect_assoc()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iteration $iterator --\n";

  // Calling array_intersect_assoc() with default arguments
  var_dump( array_intersect_assoc($arr1, $arr2) );

  // Calling array_intersect_assoc() with more arguments.
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect_assoc($arr1, $arr2, $arr1) );
  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect_assoc() : assoc array with diff values to $arr1 argument ***
-- Iteration 1 --
array(0) {
}
array(0) {
}
-- Iteration 2 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
-- Iteration 3 --
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [1]=>
  int(1)
}
-- Iteration 4 --
array(1) {
  ["two"]=>
  int(2)
}
array(1) {
  ["two"]=>
  int(2)
}
-- Iteration 5 --
array(1) {
  ["float"]=>
  float(2.3333)
}
array(1) {
  ["float"]=>
  float(2.3333)
}
-- Iteration 6 --
array(2) {
  ["f1"]=>
  float(1.2)
  ["f4"]=>
  float(33333333.333)
}
array(2) {
  ["f1"]=>
  float(1.2)
  ["f4"]=>
  float(33333333.333)
}
-- Iteration 7 --
array(1) {
  [111]=>
  string(6) "	Hello"
}
array(1) {
  [111]=>
  string(6) "	Hello"
}
-- Iteration 8 --
array(1) {
  [3]=>
  string(5) "pen\n"
}
array(1) {
  [3]=>
  string(5) "pen\n"
}
-- Iteration 9 --
array(1) {
  ["heredoc"]=>
  string(11) "Hello world"
}
array(1) {
  ["heredoc"]=>
  string(11) "Hello world"
}
-- Iteration 10 --
array(2) {
  [11]=>
  object(classA)#%d (0) {
  }
  ["resource"]=>
  resource(%d) of type (stream)
}
array(2) {
  [11]=>
  object(classA)#%d (0) {
  }
  ["resource"]=>
  resource(%d) of type (stream)
}
-- Iteration 11 --
array(4) {
  [222]=>
  string(5) "fruit"
  ["resource"]=>
  resource(%d) of type (stream)
  ["int"]=>
  int(133)
  ["heredoc"]=>
  string(11) "Hello world"
}
array(4) {
  [222]=>
  string(5) "fruit"
  ["resource"]=>
  resource(%d) of type (stream)
  ["int"]=>
  int(133)
  ["heredoc"]=>
  string(11) "Hello world"
}
Done
