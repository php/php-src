--TEST--
Test array_reverse() function : usage variations - assoc. array with diff. value for 'array' argument
--INI--
precision=12
--FILE--
<?php
/* Prototype  : array array_reverse(array $array [, bool $preserve_keys])
 * Description: Return input as a new array with the order of the entries reversed
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_reverse() by giving associative arrays with different
 * values for $array argument
*/

echo "*** Testing array_reverse() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
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

// initializing the array
$arrays = array (

       // empty array
/*1*/  array(),

       // arrays with integer values
       array('0' => 0),
       array("1" => 1),
       array("one" => 1, 'two' => 2, "three" => 3, 4 => 4),

       // arrays with float values
/*5*/  array("float" => 2.3333),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => 33333333.333333),

       // arrays with string values
       array(111 => "\tHello", "red" => "col\tor", 2 => "\v\fworld", 3.3 =>  "pen\n"),
/*8*/  array(111 => '\tHello', "red" => 'col\tor', 2 => '\v\fworld', 3.3 =>  'pen\n'),
       array(1 => "hello", "heredoc" => $heredoc),

       // array with object, unset variable and resource variable
       array(11 => new classA(), "unset" => @$unset_var, "resource" => $fp),

       // array with mixed values
/*11*/ array(1 => 'hello', 2 => new classA(), 222 => "fruit", 'resource' => $fp, "int" => 133, "float" => 444.432, "unset" => @$unset_var, "heredoc" => $heredoc)
);

// loop through the various elements of $arrays to test array_reverse()
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";
  // with default argument
  echo "- default argument -\n";
  var_dump( array_reverse($array) );
  // with $preserve_keys argument
  echo "- \$preserve keys = true -\n";
  var_dump( array_reverse($array, true) );
  echo "- \$preserve_keys = false -\n";
  var_dump( array_reverse($array, false) );
  $iterator++;
};

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_reverse() : usage variations ***
-- Iteration 1 --
- default argument -
array(0) {
}
- $preserve keys = true -
array(0) {
}
- $preserve_keys = false -
array(0) {
}
-- Iteration 2 --
- default argument -
array(1) {
  [0]=>
  int(0)
}
- $preserve keys = true -
array(1) {
  [0]=>
  int(0)
}
- $preserve_keys = false -
array(1) {
  [0]=>
  int(0)
}
-- Iteration 3 --
- default argument -
array(1) {
  [0]=>
  int(1)
}
- $preserve keys = true -
array(1) {
  [1]=>
  int(1)
}
- $preserve_keys = false -
array(1) {
  [0]=>
  int(1)
}
-- Iteration 4 --
- default argument -
array(4) {
  [0]=>
  int(4)
  ["three"]=>
  int(3)
  ["two"]=>
  int(2)
  ["one"]=>
  int(1)
}
- $preserve keys = true -
array(4) {
  [4]=>
  int(4)
  ["three"]=>
  int(3)
  ["two"]=>
  int(2)
  ["one"]=>
  int(1)
}
- $preserve_keys = false -
array(4) {
  [0]=>
  int(4)
  ["three"]=>
  int(3)
  ["two"]=>
  int(2)
  ["one"]=>
  int(1)
}
-- Iteration 5 --
- default argument -
array(1) {
  ["float"]=>
  float(2.3333)
}
- $preserve keys = true -
array(1) {
  ["float"]=>
  float(2.3333)
}
- $preserve_keys = false -
array(1) {
  ["float"]=>
  float(2.3333)
}
-- Iteration 6 --
- default argument -
array(4) {
  ["f4"]=>
  float(33333333.3333)
  [0]=>
  float(4.8999992284)
  ["f2"]=>
  float(3.33)
  ["f1"]=>
  float(1.2)
}
- $preserve keys = true -
array(4) {
  ["f4"]=>
  float(33333333.3333)
  [3]=>
  float(4.8999992284)
  ["f2"]=>
  float(3.33)
  ["f1"]=>
  float(1.2)
}
- $preserve_keys = false -
array(4) {
  ["f4"]=>
  float(33333333.3333)
  [0]=>
  float(4.8999992284)
  ["f2"]=>
  float(3.33)
  ["f1"]=>
  float(1.2)
}
-- Iteration 7 --
- default argument -
array(4) {
  [0]=>
  string(4) "pen
"
  [1]=>
  string(7) "world"
  ["red"]=>
  string(6) "col	or"
  [2]=>
  string(6) "	Hello"
}
- $preserve keys = true -
array(4) {
  [3]=>
  string(4) "pen
"
  [2]=>
  string(7) "world"
  ["red"]=>
  string(6) "col	or"
  [111]=>
  string(6) "	Hello"
}
- $preserve_keys = false -
array(4) {
  [0]=>
  string(4) "pen
"
  [1]=>
  string(7) "world"
  ["red"]=>
  string(6) "col	or"
  [2]=>
  string(6) "	Hello"
}
-- Iteration 8 --
- default argument -
array(4) {
  [0]=>
  string(5) "pen\n"
  [1]=>
  string(9) "\v\fworld"
  ["red"]=>
  string(7) "col\tor"
  [2]=>
  string(7) "\tHello"
}
- $preserve keys = true -
array(4) {
  [3]=>
  string(5) "pen\n"
  [2]=>
  string(9) "\v\fworld"
  ["red"]=>
  string(7) "col\tor"
  [111]=>
  string(7) "\tHello"
}
- $preserve_keys = false -
array(4) {
  [0]=>
  string(5) "pen\n"
  [1]=>
  string(9) "\v\fworld"
  ["red"]=>
  string(7) "col\tor"
  [2]=>
  string(7) "\tHello"
}
-- Iteration 9 --
- default argument -
array(2) {
  ["heredoc"]=>
  string(11) "Hello world"
  [0]=>
  string(5) "hello"
}
- $preserve keys = true -
array(2) {
  ["heredoc"]=>
  string(11) "Hello world"
  [1]=>
  string(5) "hello"
}
- $preserve_keys = false -
array(2) {
  ["heredoc"]=>
  string(11) "Hello world"
  [0]=>
  string(5) "hello"
}
-- Iteration 10 --
- default argument -
array(3) {
  ["resource"]=>
  resource(%d) of type (stream)
  ["unset"]=>
  NULL
  [0]=>
  object(classA)#%d (0) {
  }
}
- $preserve keys = true -
array(3) {
  ["resource"]=>
  resource(%d) of type (stream)
  ["unset"]=>
  NULL
  [11]=>
  object(classA)#%d (0) {
  }
}
- $preserve_keys = false -
array(3) {
  ["resource"]=>
  resource(%d) of type (stream)
  ["unset"]=>
  NULL
  [0]=>
  object(classA)#%d (0) {
  }
}
-- Iteration 11 --
- default argument -
array(8) {
  ["heredoc"]=>
  string(11) "Hello world"
  ["unset"]=>
  NULL
  ["float"]=>
  float(444.432)
  ["int"]=>
  int(133)
  ["resource"]=>
  resource(%d) of type (stream)
  [0]=>
  string(5) "fruit"
  [1]=>
  object(classA)#%d (0) {
  }
  [2]=>
  string(5) "hello"
}
- $preserve keys = true -
array(8) {
  ["heredoc"]=>
  string(11) "Hello world"
  ["unset"]=>
  NULL
  ["float"]=>
  float(444.432)
  ["int"]=>
  int(133)
  ["resource"]=>
  resource(%d) of type (stream)
  [222]=>
  string(5) "fruit"
  [2]=>
  object(classA)#%d (0) {
  }
  [1]=>
  string(5) "hello"
}
- $preserve_keys = false -
array(8) {
  ["heredoc"]=>
  string(11) "Hello world"
  ["unset"]=>
  NULL
  ["float"]=>
  float(444.432)
  ["int"]=>
  int(133)
  ["resource"]=>
  resource(%d) of type (stream)
  [0]=>
  string(5) "fruit"
  [1]=>
  object(classA)#%d (0) {
  }
  [2]=>
  string(5) "hello"
}
Done
