--TEST--
Test str_replace() function - test search values
--INI--
precision=14
--FILE--
<?php
echo "\n*** Testing str_replace() with various search values ***";
$search_arr = [
    1,
    0,
    -1,
    "1",
    "0",
    "-1",
    array(),
    "php",
    "",
];

$i = 0;
/* loop through to replace the matched elements in the array */
foreach( $search_arr as $value ) {
  echo "\n-- Iteration $i --\n";
  /* replace the string in array */
  var_dump( str_replace($value, "FOUND", $search_arr, $count) );
  var_dump( $count );
  $i++;
}

?>
--EXPECTF--
*** Testing str_replace() with various search values ***
-- Iteration 0 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(5) "FOUND"
  [1]=>
  string(1) "0"
  [2]=>
  string(6) "-FOUND"
  [3]=>
  string(5) "FOUND"
  [4]=>
  string(1) "0"
  [5]=>
  string(6) "-FOUND"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(4)

-- Iteration 1 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "FOUND"
  [2]=>
  string(2) "-1"
  [3]=>
  string(1) "1"
  [4]=>
  string(5) "FOUND"
  [5]=>
  string(2) "-1"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(2)

-- Iteration 2 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(5) "FOUND"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "0"
  [5]=>
  string(5) "FOUND"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(2)

-- Iteration 3 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(5) "FOUND"
  [1]=>
  string(1) "0"
  [2]=>
  string(6) "-FOUND"
  [3]=>
  string(5) "FOUND"
  [4]=>
  string(1) "0"
  [5]=>
  string(6) "-FOUND"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(4)

-- Iteration 4 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "FOUND"
  [2]=>
  string(2) "-1"
  [3]=>
  string(1) "1"
  [4]=>
  string(5) "FOUND"
  [5]=>
  string(2) "-1"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(2)

-- Iteration 5 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(5) "FOUND"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "0"
  [5]=>
  string(5) "FOUND"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(2)

-- Iteration 6 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "-1"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "0"
  [5]=>
  string(2) "-1"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(0)

-- Iteration 7 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "-1"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "0"
  [5]=>
  string(2) "-1"
  [6]=>
  string(5) "Array"
  [7]=>
  string(5) "FOUND"
  [8]=>
  string(0) ""
}
int(1)

-- Iteration 8 --

Warning: Array to string conversion in %s on line %d
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "-1"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "0"
  [5]=>
  string(2) "-1"
  [6]=>
  string(5) "Array"
  [7]=>
  string(3) "php"
  [8]=>
  string(0) ""
}
int(0)
