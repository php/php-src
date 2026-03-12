--TEST--
Test join() function : usage variations - different values for 'pieces' argument
--FILE--
<?php
/*
 * test join() by giving different pieces values
*/

echo "*** Testing join() : usage variations ***\n";

$pieces_arrays = array (
  array(1, 2), // array with default keys and numrice values
  array(1.1, 2.2), // array with default keys & float values
  array( array(2), array(1)), // sub arrays
  array(false,true), // array with default keys and boolean values
  array(), // empty array
  array(NULL), // array with NULL
  array("a","aaaa","b","bbbb","c","ccccc"),

  // associative arrays
  array(1 => "one", 2 => "two", 3 => "three"),  // explicit numeric keys, string values
  array("one" => 1, "two" => 2, "three" => 3 ),  // string keys & numeric values
  array( 1 => 10, 2 => 20, 4 => 40, 3 => 30),  // explicit numeric keys and numeric values
  array( "one" => "ten", "two" => "twenty", "three" => "thirty"),  // string key/value
  array("one" => 1, 2 => "two", 4 => "four"),  //mixed

  // associative array, containing empty/boolean values as key/value
  array(true => "true", false => "false", "false" => false, "true" => true),
  array("" => "emptyd", '' => 'emptys', "emptyd" => "", 'emptys' => ''),
  array(1 => '', 2 => "", 5 => false, 6 => true),
  array('' => 1, "" => 2, false => 5, true => 6),

  // array with repetitive keys
  array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);

// a multichar glue value
$glue = "], [";

// loop through each $pieces_arrays element and call join()
$iteration = 1;
for($index = 0; $index < count($pieces_arrays); $index ++) {
  echo "-- Iteration $iteration --\n";
  var_dump( join($glue, $pieces_arrays[$index]) );
  $iteration ++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : usage variations ***
-- Iteration 1 --
string(6) "1], [2"
-- Iteration 2 --
string(10) "1.1], [2.2"
-- Iteration 3 --

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
string(14) "Array], [Array"
-- Iteration 4 --
string(5) "], [1"
-- Iteration 5 --
string(0) ""
-- Iteration 6 --
string(0) ""
-- Iteration 7 --
string(36) "a], [aaaa], [b], [bbbb], [c], [ccccc"
-- Iteration 8 --
string(19) "one], [two], [three"
-- Iteration 9 --
string(11) "1], [2], [3"
-- Iteration 10 --
string(20) "10], [20], [40], [30"
-- Iteration 11 --
string(23) "ten], [twenty], [thirty"
-- Iteration 12 --
string(16) "1], [two], [four"
-- Iteration 13 --
string(22) "true], [false], [], [1"
-- Iteration 14 --
string(14) "emptys], [], ["
-- Iteration 15 --
string(13) "], [], [], [1"
-- Iteration 16 --
string(11) "2], [5], [6"
-- Iteration 17 --
string(13) "10], [20], [3"
Done
