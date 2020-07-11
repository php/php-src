--TEST--
Test array_unshift() function : usage variations - double quoted strings for 'var' argument
--FILE--
<?php
/*
 * Testing the functionality of array_unshift() by passing different
 * double quoted strings for $var argument that is prepended to the array
 * passed through $array argument
*/

echo "*** Testing array_unshift() : double quoted strings for \$var argument ***\n";

// array to be passed to $array argument
$array = array('f' => "first", "s" => 'second', 1, 2.222);

// different variations of double quoted strings to be passed to $var argument
$vars = array (
  "\$ -> This represents the dollar sign. hello dollar!!!",
  "\t\r\v The quick brown fo\fx jumped over the lazy dog",
  "This is a text with special chars: \!\@\#\$\%\^\&\*\(\)\\",
  "hello world\\t",
  "This is \ta text in bold letters\r\s\malong with slashes\n : HELLO WORLD\t"
);

// loop through the various elements of $arrays to test array_unshift()
$iterator = 1;
foreach($vars as $var) {
  echo "-- Iteration $iterator --\n";
  $temp_array = $array;  // assign $array to another temporary $temp_array

  /* with default argument */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
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
--EXPECT--
*** Testing array_unshift() : double quoted strings for $var argument ***
-- Iteration 1 --
int(5)
array(5) {
  [0]=>
  string(53) "$ -> This represents the dollar sign. hello dollar!!!"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  string(53) "$ -> This represents the dollar sign. hello dollar!!!"
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 2 --
int(5)
array(5) {
  [0]=>
  string(49) "	 The quick brown fox jumped over the lazy dog"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  string(49) "	 The quick brown fox jumped over the lazy dog"
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 3 --
int(5)
array(5) {
  [0]=>
  string(55) "This is a text with special chars: \!\@\#$\%\^\&\*\(\)\"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  string(55) "This is a text with special chars: \!\@\#$\%\^\&\*\(\)\"
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 4 --
int(5)
array(5) {
  [0]=>
  string(13) "hello world\t"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  string(13) "hello world\t"
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 5 --
int(5)
array(5) {
  [0]=>
  string(70) "This is 	a text in bold letters\s\malong with slashes
 : HELLO WORLD	"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  string(70) "This is 	a text in bold letters\s\malong with slashes
 : HELLO WORLD	"
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
Done
