--TEST--
Test array_unshift() function : usage variations - assoc. array with diff. keys for 'array' argument
--FILE--
<?php
/* Prototype  : int array_unshift(array $array, mixed $var [, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unshift() by passing different
 * associative arrays having different possible keys to $array argument.
 * The $var argument passed is a fixed value
*/

echo "*** Testing array_unshift() : associative array with different keys ***\n";

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

// initializing $var argument
$var = 10;

// different variations of associative arrays to be passed to $array argument
$arrays = array (

       // empty array
/*1*/  array(),

       // arrays with integer keys
       array(0 => "0"),
       array(1 => "1"),
       array(1 => "1", 2 => "2", 3 => "3", 4 => "4"),

       // arrays with float keys
/*5*/  array(2.3333 => "float"),
       array(1.2 => "f1", 3.33 => "f2",
             4.89999922839999 => "f3",
             33333333.333333 => "f4"),

       // arrays with string keys
/*7*/  array('\tHello' => 111, 're\td' => "color",
             '\v\fworld' => 2.2, 'pen\n' => 33),
       array("\tHello" => 111, "re\td" => "color",
             "\v\fworld" => 2.2, "pen\n" => 33),
       array("hello", $heredoc => "string"), // heredoc

       // array with object, unset variable and resource variable
       array(new classA() => 11, @$unset_var => "hello", $fp => 'resource'),

       // array with mixed keys
/*11*/ array('hello' => 1, new classA() => 2, "fruit" => 2.2,
             $fp => 'resource', 133 => "int", 444.432 => "float",
             @$unset_var => "unset", $heredoc => "heredoc")
);

// loop through the various elements of $arrays to test array_unshift()
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";

  /* with default argument */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var) );

  // dump the resulting array
  var_dump($temp_array);

  /* with optional arguments */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var, "hello", 'world') );

  // dump the resulting array
  var_dump($temp_array);
  $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : associative array with different keys ***

Warning: Illegal offset type in %s on line %d

Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d

Warning: Illegal offset type in %s on line %d

Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
-- Iteration 1 --
int(1)
array(1) {
  [0]=>
  int(10)
}
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
}
-- Iteration 2 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  string(1) "0"
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(1) "0"
}
-- Iteration 3 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  string(1) "1"
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(1) "1"
}
-- Iteration 4 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "3"
  [4]=>
  string(1) "4"
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "2"
  [5]=>
  string(1) "3"
  [6]=>
  string(1) "4"
}
-- Iteration 5 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  string(5) "float"
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(5) "float"
}
-- Iteration 6 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(2) "f1"
  [2]=>
  string(2) "f2"
  [3]=>
  string(2) "f3"
  [4]=>
  string(2) "f4"
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(2) "f1"
  [4]=>
  string(2) "f2"
  [5]=>
  string(2) "f3"
  [6]=>
  string(2) "f4"
}
-- Iteration 7 --
int(5)
array(5) {
  [0]=>
  int(10)
  ["\tHello"]=>
  int(111)
  ["re\td"]=>
  string(5) "color"
  ["\v\fworld"]=>
  float(2.2)
  ["pen\n"]=>
  int(33)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["\tHello"]=>
  int(111)
  ["re\td"]=>
  string(5) "color"
  ["\v\fworld"]=>
  float(2.2)
  ["pen\n"]=>
  int(33)
}
-- Iteration 8 --
int(5)
array(5) {
  [0]=>
  int(10)
  ["	Hello"]=>
  int(111)
  ["re	d"]=>
  string(5) "color"
  ["world"]=>
  float(2.2)
  ["pen
"]=>
  int(33)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["	Hello"]=>
  int(111)
  ["re	d"]=>
  string(5) "color"
  ["world"]=>
  float(2.2)
  ["pen
"]=>
  int(33)
}
-- Iteration 9 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  ["Hello world"]=>
  string(6) "string"
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(5) "hello"
  ["Hello world"]=>
  string(6) "string"
}
-- Iteration 10 --
int(3)
array(3) {
  [0]=>
  int(10)
  [""]=>
  string(5) "hello"
  [1]=>
  string(8) "resource"
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [""]=>
  string(5) "hello"
  [3]=>
  string(8) "resource"
}
-- Iteration 11 --
int(8)
array(8) {
  [0]=>
  int(10)
  ["hello"]=>
  int(1)
  ["fruit"]=>
  float(2.2)
  [1]=>
  string(8) "resource"
  [2]=>
  string(3) "int"
  [3]=>
  string(5) "float"
  [""]=>
  string(5) "unset"
  ["Hello world"]=>
  string(7) "heredoc"
}
int(10)
array(10) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["hello"]=>
  int(1)
  ["fruit"]=>
  float(2.2)
  [3]=>
  string(8) "resource"
  [4]=>
  string(3) "int"
  [5]=>
  string(5) "float"
  [""]=>
  string(5) "unset"
  ["Hello world"]=>
  string(7) "heredoc"
}
Done
