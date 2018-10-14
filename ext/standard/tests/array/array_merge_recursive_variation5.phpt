--TEST--
Test array_merge_recursive() function : usage variations - associative array with different values
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_merge_recursive() by passing different
 * associative arrays having different values to $arr1 argument.
*/

echo "*** Testing array_merge_recursive() : assoc. array with diff. values to \$arr1 argument ***\n";

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

// different associative arrays to be passed to $arr1 argument
$arrays = array (
// arrays with integer values
/*1*/  array('0' => 0, '1' => 0),
       array("one" => 1, 'two' => 2, "three" => 1, 4 => 1),

       // arrays with float values
/*3*/  array("f1" => 2.3333, "f2" => 2.3333, "f3" => array(1.1, 2.22)),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => array(1.2, 'f4' => 1.2)),

       // arrays with string values
/*5*/  array(111 => "\tHello", "array" => "col\tor", 2 => "\v\fworld", 3.3 =>  "\tHello"),
       array(111 => '\tHello', 'array' => 'col\tor', 2 => '\v\fworld', 3.3 =>  '\tHello'),
       array(1 => "hello", "string" => $heredoc, $heredoc),

       // array with object, unset variable and resource variable
/*8*/  array(11 => new classA(), "string" => @$unset_var, "resource" => $fp, new classA(), $fp),
);

// initialise the second array
$arr2 = array( 1 => "one", 2, "string" => "hello", "array" => array("a", "b", "c"));

// loop through each sub array of $arrays and check the behavior of array_merge_recursive()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iteration $iterator --\n";

  // with default argument
  echo "-- With default argument --\n";
  var_dump( array_merge_recursive($arr1) );

  // with more arguments
  echo "-- With more arguments --\n";
  var_dump( array_merge_recursive($arr1, $arr2) );

  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_merge_recursive() : assoc. array with diff. values to $arr1 argument ***
-- Iteration 1 --
-- With default argument --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
-- With more arguments --
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  string(3) "one"
  [3]=>
  int(2)
  ["string"]=>
  string(5) "hello"
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
-- Iteration 2 --
-- With default argument --
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(1)
  [0]=>
  int(1)
}
-- With more arguments --
array(8) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(1)
  [0]=>
  int(1)
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["string"]=>
  string(5) "hello"
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
-- Iteration 3 --
-- With default argument --
array(3) {
  ["f1"]=>
  float(2.3333)
  ["f2"]=>
  float(2.3333)
  ["f3"]=>
  array(2) {
    [0]=>
    float(1.1)
    [1]=>
    float(2.22)
  }
}
-- With more arguments --
array(7) {
  ["f1"]=>
  float(2.3333)
  ["f2"]=>
  float(2.3333)
  ["f3"]=>
  array(2) {
    [0]=>
    float(1.1)
    [1]=>
    float(2.22)
  }
  [0]=>
  string(3) "one"
  [1]=>
  int(2)
  ["string"]=>
  string(5) "hello"
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
-- Iteration 4 --
-- With default argument --
array(4) {
  ["f1"]=>
  float(1.2)
  ["f2"]=>
  float(3.33)
  [0]=>
  float(4.8999992284)
  ["f4"]=>
  array(2) {
    [0]=>
    float(1.2)
    ["f4"]=>
    float(1.2)
  }
}
-- With more arguments --
array(8) {
  ["f1"]=>
  float(1.2)
  ["f2"]=>
  float(3.33)
  [0]=>
  float(4.8999992284)
  ["f4"]=>
  array(2) {
    [0]=>
    float(1.2)
    ["f4"]=>
    float(1.2)
  }
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["string"]=>
  string(5) "hello"
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
-- Iteration 5 --
-- With default argument --
array(4) {
  [0]=>
  string(6) "	Hello"
  ["array"]=>
  string(6) "col	or"
  [1]=>
  string(7) "world"
  [2]=>
  string(6) "	Hello"
}
-- With more arguments --
array(7) {
  [0]=>
  string(6) "	Hello"
  ["array"]=>
  array(4) {
    [0]=>
    string(6) "col	or"
    [1]=>
    string(1) "a"
    [2]=>
    string(1) "b"
    [3]=>
    string(1) "c"
  }
  [1]=>
  string(7) "world"
  [2]=>
  string(6) "	Hello"
  [3]=>
  string(3) "one"
  [4]=>
  int(2)
  ["string"]=>
  string(5) "hello"
}
-- Iteration 6 --
-- With default argument --
array(4) {
  [0]=>
  string(7) "\tHello"
  ["array"]=>
  string(7) "col\tor"
  [1]=>
  string(9) "\v\fworld"
  [2]=>
  string(7) "\tHello"
}
-- With more arguments --
array(7) {
  [0]=>
  string(7) "\tHello"
  ["array"]=>
  array(4) {
    [0]=>
    string(7) "col\tor"
    [1]=>
    string(1) "a"
    [2]=>
    string(1) "b"
    [3]=>
    string(1) "c"
  }
  [1]=>
  string(9) "\v\fworld"
  [2]=>
  string(7) "\tHello"
  [3]=>
  string(3) "one"
  [4]=>
  int(2)
  ["string"]=>
  string(5) "hello"
}
-- Iteration 7 --
-- With default argument --
array(3) {
  [0]=>
  string(5) "hello"
  ["string"]=>
  string(11) "Hello world"
  [1]=>
  string(11) "Hello world"
}
-- With more arguments --
array(6) {
  [0]=>
  string(5) "hello"
  ["string"]=>
  array(2) {
    [0]=>
    string(11) "Hello world"
    [1]=>
    string(5) "hello"
  }
  [1]=>
  string(11) "Hello world"
  [2]=>
  string(3) "one"
  [3]=>
  int(2)
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
-- Iteration 8 --
-- With default argument --
array(5) {
  [0]=>
  object(classA)#%d (0) {
  }
  ["string"]=>
  NULL
  ["resource"]=>
  resource(%d) of type (stream)
  [1]=>
  object(classA)#%d (0) {
  }
  [2]=>
  resource(%d) of type (stream)
}
-- With more arguments --
array(8) {
  [0]=>
  object(classA)#%d (0) {
  }
  ["string"]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(5) "hello"
  }
  ["resource"]=>
  resource(%d) of type (stream)
  [1]=>
  object(classA)#%d (0) {
  }
  [2]=>
  resource(%d) of type (stream)
  [3]=>
  string(3) "one"
  [4]=>
  int(2)
  ["array"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
Done
