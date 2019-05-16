--TEST--
Test array_intersect_assoc() function : usage variations - assoc array with diff keys for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_intersect_assoc(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments.
 * Keys are used to do more restrictive check
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_intersect_assoc() by passing different
 * associative arrays having different possible keys to $arr1 argument.
 * The $arr2 argument passed is a fixed array
*/

echo "*** Testing array_intersect_assoc() : assoc array with diff keys to \$arr1 argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a heredoc string
$heredoc = <<<EOT
Hello world
EOT;

// different variations of associative arrays to be passed to $arr1 argument
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
/*10*/ array(@$unset_var => "hello"),

       // array with mixed keys
/*11*/ array('hello' => 1, "fruit" => 2.2,
              133 => "int", 444.432 => "float",
             @$unset_var => "unset", $heredoc => "heredoc")
);

// array to be passsed to $arr2 argument
$arr2 = array(0 => 0, 2 => "float", 4 => "f3", 33333333 => "f4",
              "\tHello" => 111, 2.2, 'color', "Hello world" => "string",
              "pen\n" => 33,  133 => "int");

// loop through each sub-array within $arrrays to check the behavior of array_intersect_assoc()
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

echo "Done";
?>
--EXPECT--
*** Testing array_intersect_assoc() : assoc array with diff keys to $arr1 argument ***
-- Iteration 1 --
array(0) {
}
array(0) {
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "0"
}
-- Iteration 3 --
array(0) {
}
array(0) {
}
-- Iteration 4 --
array(0) {
}
array(0) {
}
-- Iteration 5 --
array(1) {
  [2]=>
  string(5) "float"
}
array(1) {
  [2]=>
  string(5) "float"
}
-- Iteration 6 --
array(2) {
  [4]=>
  string(2) "f3"
  [33333333]=>
  string(2) "f4"
}
array(2) {
  [4]=>
  string(2) "f3"
  [33333333]=>
  string(2) "f4"
}
-- Iteration 7 --
array(0) {
}
array(0) {
}
-- Iteration 8 --
array(2) {
  ["	Hello"]=>
  int(111)
  ["pen
"]=>
  int(33)
}
array(2) {
  ["	Hello"]=>
  int(111)
  ["pen
"]=>
  int(33)
}
-- Iteration 9 --
array(1) {
  ["Hello world"]=>
  string(6) "string"
}
array(1) {
  ["Hello world"]=>
  string(6) "string"
}
-- Iteration 10 --
array(0) {
}
array(0) {
}
-- Iteration 11 --
array(1) {
  [133]=>
  string(3) "int"
}
array(1) {
  [133]=>
  string(3) "int"
}
Done
