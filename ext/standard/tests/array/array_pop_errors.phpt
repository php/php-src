--TEST--
Test array_pop() function (errors)
--FILE--
<?php

$empty_array = array();
$number = 5;
$str = "abc";


/* Various combinations of arrays to be used for the test */
$mixed_array = array(
  array(),
  array( 1,2,3,4,5,6,7,8,9 ),
  array( "One", "_Two", "Three", "Four", "Five" ),
  array( 6, "six", 7, "seven", 8, "eight", 9, "nine" ),
  array( "a" => "aaa", "A" => "AAA", "c" => "ccc", "d" => "ddd", "e" => "eee" ),
  array( "1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five" ),
  array( 1 => "one", 2 => "two", 3 => 7, 4 => "four", 5 => "five" ),
  array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2.4 => "float", "F" => "FFF",
         "blank" => "", 3.7 => 3.7, 5.4 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 ),
  array( 12, "name", 'age', '45' ),
  array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array() ),
  array( "one" => 1, "one" => 2, "three" => 3, 3, 4, 3 => 33, 4 => 44, 5, 6,
          5.4 => 54, 5.7 => 57, "5.4" => 554, "5.7" => 557 )
);

/* Testing Error Conditions */
echo "\n*** Testing Error Conditions ***\n";

/* Zero argument  */
var_dump( array_pop() );

/* Scalar argument */
var_dump( array_pop($number) );

/* String argument */
var_dump( array_pop($str) );

/* Invalid Number of arguments */
var_dump( array_pop($mixed_array[1],$mixed_array[2]) );

/* Empty Array as argument */
var_dump( array_pop($empty_array) );

echo"\nDone";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: array_pop() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: array_pop() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_pop() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_pop() expects exactly 1 parameter, 2 given in %s on line %d
NULL
NULL

Done
