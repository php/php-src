--TEST--
Test array_merge_recursive() function : usage variations - associative array with different keys
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_merge_recursive() by passing different
 * associative arrays having different keys to $arr1 argument.
*/

echo "*** Testing array_merge_recursive() : assoc. array with diff. keys to \$arr1 argument ***\n";

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
/*1*/  // arrays with integer keys
       array(0 => "0", 1 => array(1 => "one")),
       array(1 => "1", 2 => array(1 => "one", 2 => "two", 3 => 1, 4 => "4")),

       // arrays with float keys
/*3*/  array(2.3333 => "float", 44.44 => array(1.1 => "float")),
       array(1.2 => "f1", 3.33 => "f2", 4.89999922839999 => array(1.1 => "f1"), 3333333.333333 => "f4"),

       // arrays with string keys
/*5*/  array('\tHello' => array("hello", 'world'), '\v\fworld' => 2.2, 'pen\n' => 111),
       array("\tHello" => array("hello", 'world'), "\v\fworld" => 2.2, "pen\n" => 111),
       array("hello", $heredoc => array("heredoc", 'string'), "string"),

       // array with object, unset variable and resource variable
/*8*/ array(new classA() => 11, @$unset_var => array("unset"), $fp => 'resource', 11, "hello")
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
*** Testing array_merge_recursive() : assoc. array with diff. keys to $arr1 argument ***

Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d
-- Iteration 1 --
-- With default argument --
array(2) {
  [0]=>
  string(1) "0"
  [1]=>
  array(1) {
    [1]=>
    string(3) "one"
  }
}
-- With more arguments --
array(6) {
  [0]=>
  string(1) "0"
  [1]=>
  array(1) {
    [1]=>
    string(3) "one"
  }
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
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  array(4) {
    [1]=>
    string(3) "one"
    [2]=>
    string(3) "two"
    [3]=>
    int(1)
    [4]=>
    string(1) "4"
  }
}
-- With more arguments --
array(6) {
  [0]=>
  string(1) "1"
  [1]=>
  array(4) {
    [1]=>
    string(3) "one"
    [2]=>
    string(3) "two"
    [3]=>
    int(1)
    [4]=>
    string(1) "4"
  }
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
-- Iteration 3 --
-- With default argument --
array(2) {
  [0]=>
  string(5) "float"
  [1]=>
  array(1) {
    [1]=>
    string(5) "float"
  }
}
-- With more arguments --
array(6) {
  [0]=>
  string(5) "float"
  [1]=>
  array(1) {
    [1]=>
    string(5) "float"
  }
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
-- Iteration 4 --
-- With default argument --
array(4) {
  [0]=>
  string(2) "f1"
  [1]=>
  string(2) "f2"
  [2]=>
  array(1) {
    [1]=>
    string(2) "f1"
  }
  [3]=>
  string(2) "f4"
}
-- With more arguments --
array(8) {
  [0]=>
  string(2) "f1"
  [1]=>
  string(2) "f2"
  [2]=>
  array(1) {
    [1]=>
    string(2) "f1"
  }
  [3]=>
  string(2) "f4"
  [4]=>
  string(3) "one"
  [5]=>
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
array(3) {
  ["\tHello"]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  ["\v\fworld"]=>
  float(2.2)
  ["pen\n"]=>
  int(111)
}
-- With more arguments --
array(7) {
  ["\tHello"]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  ["\v\fworld"]=>
  float(2.2)
  ["pen\n"]=>
  int(111)
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
-- Iteration 6 --
-- With default argument --
array(3) {
  ["	Hello"]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  ["world"]=>
  float(2.2)
  ["pen
"]=>
  int(111)
}
-- With more arguments --
array(7) {
  ["	Hello"]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  ["world"]=>
  float(2.2)
  ["pen
"]=>
  int(111)
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
-- Iteration 7 --
-- With default argument --
array(3) {
  [0]=>
  string(5) "hello"
  ["Hello world"]=>
  array(2) {
    [0]=>
    string(7) "heredoc"
    [1]=>
    string(6) "string"
  }
  [1]=>
  string(6) "string"
}
-- With more arguments --
array(7) {
  [0]=>
  string(5) "hello"
  ["Hello world"]=>
  array(2) {
    [0]=>
    string(7) "heredoc"
    [1]=>
    string(6) "string"
  }
  [1]=>
  string(6) "string"
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
-- Iteration 8 --
-- With default argument --
array(3) {
  [""]=>
  array(1) {
    [0]=>
    string(5) "unset"
  }
  [0]=>
  int(11)
  [1]=>
  string(5) "hello"
}
-- With more arguments --
array(7) {
  [""]=>
  array(1) {
    [0]=>
    string(5) "unset"
  }
  [0]=>
  int(11)
  [1]=>
  string(5) "hello"
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
Done
