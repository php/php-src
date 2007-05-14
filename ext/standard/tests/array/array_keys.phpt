--TEST--
Test array_keys() function
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
Prototype: array array_keys( array $input [, mixed $search_value [, 
                             bool $strict]]);
Description: Return all the keys of an array
*/

echo "*** Testing array_keys() on basic array operation ***\n";
$basic_arr = array("a" => 1, "b" => 2, 2.0 => 2.0, -23.45 => "asdasd",
                   array(1,2,3));
var_dump(array_keys($basic_arr));

echo "\n*** Testing array_keys() on various arrays ***";
$arrays = array(
  array(), 
  array(0),
  array( array() ), 
  array("Hello" => "World"), 
  array("" => ""),  
  array(1,2,3, "d" => array(4,6, "d")),
  array("a" => 1, "b" => 2, "c" =>3, "d" => array()),
  array(0 => 0, 1 => 1, 2 => 2, 3 => 3),  
  array(0.001=>3.000, 1.002=>2, 1.999=>3, "a"=>3, 3=>5, "5"=>3.000),
  array(TRUE => TRUE, FALSE => FALSE, NULL => NULL, "\x000", "\000"),
  array("a" => "abcd", "a" => "", "ab" => -6, "cd" => -0.5 ),
  array(0 => array(), 1=> array(0), 2 => array(1), ""=> array(),""=>"" )
);

$i = 0;
/* loop through to test array_keys() with different arrays */
foreach ($arrays as $array) {
  echo "\n-- Iteration $i --\n";
  var_dump(array_keys($array)); 
  $i++;
}

echo "\n*** Testing array_keys() on all the types other than arrays ***";
$types_arr = array(
  TRUE => TRUE,
  FALSE => FALSE,
  1 => 1,
  0 => 0,
  -1 => -1, 
  "1" => "1",
  "0" => "0",
  "-1" => "-1",
  NULL,
  array(),
  "php" => "php",
  "" => ""
);
$values = array(TRUE, FALSE, 1, 0, -1, "1", "0", "-1",  NULL, array(), "php", "");
foreach ($values as $value){
  echo "\n-- Loose type checking --\n";
  var_dump(array_keys($types_arr, $value)); 
  echo "\n-- strict type checking --\n";
  var_dump(array_keys($types_arr, $value, TRUE));
}

echo "\n*** Testing array_keys() with resource type ***\n";
$resource1 = fopen( __FILE__, "r");
$resource2 = opendir( "." );

/* creating an array with resource types as elements */
$arr_resource = array($resource1, $resource2); 

var_dump(array_keys($arr_resource, $resource1));  // loose type checking 
var_dump(array_keys($arr_resource, $resource1, TRUE));  // strict type checking
var_dump(array_keys($arr_resource, $resource2));  // loose type checking
var_dump(array_keys($arr_resource, $resource2, TRUE));  // strict type checking

echo "\n*** Testing array_keys() on range of values ***\n";
$arr_range = array(
  2147483647 => 1,
  2147483648 => 2,
  -2147483647 => 3, 
  -2147483648 => 4,
  -2147483649 => 5,
  -0 => 6,
  0 => 7
);
var_dump(array_keys($arr_range));

echo "\n*** Testing array_keys() on an array created on the fly ***\n";
var_dump(array_keys(array("a" => 1, "b" => 2, "c" => 3)));
var_dump(array_keys(array()));  // null array

echo "\n*** Testing error conditions ***";
var_dump(array_keys(100));
var_dump(array_keys("string"));
var_dump(array_keys(new stdclass));  // object
var_dump(array_keys());  // Zero arguments
var_dump(array_keys(array(), "", TRUE, 100));  // args > expected
var_dump(array_keys(array(1,2,3, array() => array())));  // (W)illegal offset

echo "Done\n";

--CLEAN--
/* Closing the resource handles */
fclose( $resource1 );
closedir( $resource2 );
?>
--EXPECTF--
*** Testing array_keys() on basic array operation ***
array(5) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  int(2)
  [3]=>
  int(-23)
  [4]=>
  int(3)
}

*** Testing array_keys() on various arrays ***
-- Iteration 0 --
array(0) {
}

-- Iteration 1 --
array(1) {
  [0]=>
  int(0)
}

-- Iteration 2 --
array(1) {
  [0]=>
  int(0)
}

-- Iteration 3 --
array(1) {
  [0]=>
  string(5) "Hello"
}

-- Iteration 4 --
array(1) {
  [0]=>
  string(0) ""
}

-- Iteration 5 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  string(1) "d"
}

-- Iteration 6 --
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
}

-- Iteration 7 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}

-- Iteration 8 --
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "a"
  [3]=>
  int(3)
  [4]=>
  int(5)
}

-- Iteration 9 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(0)
  [2]=>
  string(0) ""
  [3]=>
  int(2)
  [4]=>
  int(3)
}

-- Iteration 10 --
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(2) "ab"
  [2]=>
  string(2) "cd"
}

-- Iteration 11 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  string(0) ""
}

*** Testing array_keys() on all the types other than arrays ***
-- Loose type checking --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(-1)
  [2]=>
  string(3) "php"
}

-- strict type checking --
array(0) {
}

-- Loose type checking --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  string(0) ""
}

-- strict type checking --
array(0) {
}

-- Loose type checking --
array(1) {
  [0]=>
  int(1)
}

-- strict type checking --
array(0) {
}

-- Loose type checking --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  string(3) "php"
  [3]=>
  string(0) ""
}

-- strict type checking --
array(0) {
}

-- Loose type checking --
array(1) {
  [0]=>
  int(-1)
}

-- strict type checking --
array(0) {
}

-- Loose type checking --
array(1) {
  [0]=>
  int(1)
}

-- strict type checking --
array(1) {
  [0]=>
  int(1)
}

-- Loose type checking --
array(1) {
  [0]=>
  int(0)
}

-- strict type checking --
array(1) {
  [0]=>
  int(0)
}

-- Loose type checking --
array(1) {
  [0]=>
  int(-1)
}

-- strict type checking --
array(1) {
  [0]=>
  int(-1)
}

-- Loose type checking --
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  string(0) ""
}

-- strict type checking --
array(1) {
  [0]=>
  int(2)
}

-- Loose type checking --
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}

-- strict type checking --
array(1) {
  [0]=>
  int(3)
}

-- Loose type checking --
array(1) {
  [0]=>
  string(3) "php"
}

-- strict type checking --
array(1) {
  [0]=>
  string(3) "php"
}

-- Loose type checking --
array(2) {
  [0]=>
  int(2)
  [1]=>
  string(0) ""
}

-- strict type checking --
array(1) {
  [0]=>
  string(0) ""
}

*** Testing array_keys() with resource type ***
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}

*** Testing array_keys() on range of values ***
array(4) {
  [0]=>
  int(2147483647)
  [1]=>
  int(-2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  int(0)
}

*** Testing array_keys() on an array created on the fly ***
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(0) {
}

*** Testing error conditions ***
Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: Wrong parameter count for array_keys() in %s on line %d
NULL

Warning: Wrong parameter count for array_keys() in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
Done
