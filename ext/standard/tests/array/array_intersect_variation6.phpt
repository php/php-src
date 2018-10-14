--TEST--
Test array_intersect() function : usage variations - assoc array with diff keys for 'arr2' argument
--FILE--
<?php
/* Prototype  : array array_intersect(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_intersect() by passing different
 * associative arrays having different possible keys to $arr2 argument.
 * The $arr1 argument is a fixed array.
*/

echo "*** Testing array_intersect() : assoc array with diff keys to \$arr2 argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a heredoc string
$heredoc = <<<EOT
Hello world
EOT;

// different variations of associative arrays to be passed to $arr2 argument
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

       // array with unset variable
/*10*/ array( @$unset_var => "hello"),

       // array with mixed keys
/*11*/ array('hello' => 1,  "fruit" => 2.2,
             133 => "int", 444.432 => "float",
             @$unset_var => "unset", $heredoc => "heredoc")
);

// array to be passsed to $arr1 argument
$arr1 = array(1, "float", "f4", "hello", 2.2, 'color', "string", "pen\n", 11);

// loop through each sub-array within $arrrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $arr2) {
  echo "-- Iterator $iterator --\n";

  // Calling array_intersect() with default arguments
  var_dump( array_intersect($arr1, $arr2) );

  // Calling array_intersect() with more arguments.
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect($arr1, $arr2, $arr1) );
  $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect() : assoc array with diff keys to $arr2 argument ***
-- Iterator 1 --
array(0) {
}
array(0) {
}
-- Iterator 2 --
array(0) {
}
array(0) {
}
-- Iterator 3 --
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
-- Iterator 4 --
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
-- Iterator 5 --
array(1) {
  [1]=>
  string(5) "float"
}
array(1) {
  [1]=>
  string(5) "float"
}
-- Iterator 6 --
array(1) {
  [2]=>
  string(2) "f4"
}
array(1) {
  [2]=>
  string(2) "f4"
}
-- Iterator 7 --
array(2) {
  [4]=>
  float(2.2)
  [5]=>
  string(5) "color"
}
array(2) {
  [4]=>
  float(2.2)
  [5]=>
  string(5) "color"
}
-- Iterator 8 --
array(2) {
  [4]=>
  float(2.2)
  [5]=>
  string(5) "color"
}
array(2) {
  [4]=>
  float(2.2)
  [5]=>
  string(5) "color"
}
-- Iterator 9 --
array(2) {
  [3]=>
  string(5) "hello"
  [6]=>
  string(6) "string"
}
array(2) {
  [3]=>
  string(5) "hello"
  [6]=>
  string(6) "string"
}
-- Iterator 10 --
array(1) {
  [3]=>
  string(5) "hello"
}
array(1) {
  [3]=>
  string(5) "hello"
}
-- Iterator 11 --
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(5) "float"
  [4]=>
  float(2.2)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(5) "float"
  [4]=>
  float(2.2)
}
Done
