--TEST--
Test array_reverse() function : usage variations - assoc. array with diff. keys for 'array' argument
--FILE--
<?php
/* Prototype  : array array_reverse(array $array [, bool $preserve_keys])
 * Description: Return input as a new array with the order of the entries reversed
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_reverse() by giving associative arrays with different
 * keys for $array argument
*/

echo "*** Testing array_reverse() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
class classA{
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

       // arrays with integer keys
       array(0 => "0"),
       array(1 => "1"),
       array(1 => "1", 2 => "2", 3 => "3", 4 => "4"),

       // arrays with float keys
/*5*/  array(2.3333 => "float"),
       array(1.2 => "f1", 3.33 => "f2", 4.89999922839999 => "f3", 33333333.333333 => "f4"),

       // arrays with string keys
       array("\tHello" => 111, "re\td" => "color", "\v\fworld" => 2.2, "pen\n" => 33),
/*8*/  array("\tHello" => 111, "re\td" => "color", "\v\fworld" => 2.2, "pen\n" => 33),
       array("hello", $heredoc => "string"), // heredoc

       // array with object, unset variable and resource variable
       array(new classA() => 11, @$unset_var => "hello", $fp => 'resource'),

       // array with mixed values
/*11*/ array('hello' => 1, new classA() => 2, "fruit" => 2.2, $fp => 'resource', 133 => "int", 444.432 => "float", @$unset_var => "unset", $heredoc => "heredoc")
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

Warning: Illegal offset type in %s on line %d

Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d

Warning: Illegal offset type in %s on line %d

Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
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
  string(1) "0"
}
- $preserve keys = true -
array(1) {
  [0]=>
  string(1) "0"
}
- $preserve_keys = false -
array(1) {
  [0]=>
  string(1) "0"
}
-- Iteration 3 --
- default argument -
array(1) {
  [0]=>
  string(1) "1"
}
- $preserve keys = true -
array(1) {
  [1]=>
  string(1) "1"
}
- $preserve_keys = false -
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 4 --
- default argument -
array(4) {
  [0]=>
  string(1) "4"
  [1]=>
  string(1) "3"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "1"
}
- $preserve keys = true -
array(4) {
  [4]=>
  string(1) "4"
  [3]=>
  string(1) "3"
  [2]=>
  string(1) "2"
  [1]=>
  string(1) "1"
}
- $preserve_keys = false -
array(4) {
  [0]=>
  string(1) "4"
  [1]=>
  string(1) "3"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "1"
}
-- Iteration 5 --
- default argument -
array(1) {
  [0]=>
  string(5) "float"
}
- $preserve keys = true -
array(1) {
  [2]=>
  string(5) "float"
}
- $preserve_keys = false -
array(1) {
  [0]=>
  string(5) "float"
}
-- Iteration 6 --
- default argument -
array(4) {
  [0]=>
  string(2) "f4"
  [1]=>
  string(2) "f3"
  [2]=>
  string(2) "f2"
  [3]=>
  string(2) "f1"
}
- $preserve keys = true -
array(4) {
  [33333333]=>
  string(2) "f4"
  [4]=>
  string(2) "f3"
  [3]=>
  string(2) "f2"
  [1]=>
  string(2) "f1"
}
- $preserve_keys = false -
array(4) {
  [0]=>
  string(2) "f4"
  [1]=>
  string(2) "f3"
  [2]=>
  string(2) "f2"
  [3]=>
  string(2) "f1"
}
-- Iteration 7 --
- default argument -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
- $preserve keys = true -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
- $preserve_keys = false -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
-- Iteration 8 --
- default argument -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
- $preserve keys = true -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
- $preserve_keys = false -
array(4) {
  ["pen
"]=>
  int(33)
  ["world"]=>
  float(2.2)
  ["re	d"]=>
  string(5) "color"
  ["	Hello"]=>
  int(111)
}
-- Iteration 9 --
- default argument -
array(2) {
  ["Hello world"]=>
  string(6) "string"
  [0]=>
  string(5) "hello"
}
- $preserve keys = true -
array(2) {
  ["Hello world"]=>
  string(6) "string"
  [0]=>
  string(5) "hello"
}
- $preserve_keys = false -
array(2) {
  ["Hello world"]=>
  string(6) "string"
  [0]=>
  string(5) "hello"
}
-- Iteration 10 --
- default argument -
array(2) {
  [0]=>
  string(8) "resource"
  [""]=>
  string(5) "hello"
}
- $preserve keys = true -
array(2) {
  [5]=>
  string(8) "resource"
  [""]=>
  string(5) "hello"
}
- $preserve_keys = false -
array(2) {
  [0]=>
  string(8) "resource"
  [""]=>
  string(5) "hello"
}
-- Iteration 11 --
- default argument -
array(7) {
  ["Hello world"]=>
  string(7) "heredoc"
  [""]=>
  string(5) "unset"
  [0]=>
  string(5) "float"
  [1]=>
  string(3) "int"
  [2]=>
  string(8) "resource"
  ["fruit"]=>
  float(2.2)
  ["hello"]=>
  int(1)
}
- $preserve keys = true -
array(7) {
  ["Hello world"]=>
  string(7) "heredoc"
  [""]=>
  string(5) "unset"
  [444]=>
  string(5) "float"
  [133]=>
  string(3) "int"
  [5]=>
  string(8) "resource"
  ["fruit"]=>
  float(2.2)
  ["hello"]=>
  int(1)
}
- $preserve_keys = false -
array(7) {
  ["Hello world"]=>
  string(7) "heredoc"
  [""]=>
  string(5) "unset"
  [0]=>
  string(5) "float"
  [1]=>
  string(3) "int"
  [2]=>
  string(8) "resource"
  ["fruit"]=>
  float(2.2)
  ["hello"]=>
  int(1)
}
Done
