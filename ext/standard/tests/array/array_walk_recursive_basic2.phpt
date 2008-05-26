--TEST--
Test array_walk_recursive() function : basic functionality - associative array
--FILE--
<?php
/* Prototype  : bool array_walk_recursive(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_walk_recursive() : basic functionality ***\n";

// associative array
$fruits = array("a" => "lemon", "b" => array( "c" => "orange", "d" => "banana"), "e" => array("f" => "apple"));

// User defined callback functions
/*  Prototype : test_alter(mixed $item, mixed $key, string $prefix)
 *  Parameters : item - value in key/value pair 
 *               key - key in key/value pair
 *               prefix - string to be added 
 *  Description : alters the array values by appending prefix string
 */ 
function test_alter(&$item, $key, $prefix)
{
  // dump the arguments to check that they are passed
  // with proper type
  var_dump($item); // value
  var_dump($key);  // key
  var_dump($prefix); // additional agument passed to callback function
  echo "\n"; // new line to separate the output between each element
}

/*  Prototype : test_print(mixed $item, mixed $key)
 *  Parameters : item - value in key/value pair 
 *               key - key in key/value pair
 *  Description : prints the array values with keys
 */
function test_print($item, $key)
{
  // dump the arguments to check that they are passed
  // with proper type
  var_dump($item); // value
  var_dump($key);  // key
  echo "\n"; // new line to separate the output between each element
}

echo "-- Using array_walk_recursive with default parameters to show array contents --\n";
var_dump(array_walk_recursive($fruits, 'test_print'));

echo "-- Using array_walk_recursive with one optional parameter to modify contents --\n";
var_dump (array_walk_recursive($fruits, 'test_alter', 'fruit'));

echo "-- Using array_walk_recursive with default parameters to show modified array contents --\n";
var_dump (array_walk_recursive($fruits, 'test_print'));

echo "Done";
?>
--EXPECT--
*** Testing array_walk_recursive() : basic functionality ***
-- Using array_walk_recursive with default parameters to show array contents --
unicode(5) "lemon"
unicode(1) "a"

unicode(6) "orange"
unicode(1) "c"

unicode(6) "banana"
unicode(1) "d"

unicode(5) "apple"
unicode(1) "f"

bool(true)
-- Using array_walk_recursive with one optional parameter to modify contents --
unicode(5) "lemon"
unicode(1) "a"
unicode(5) "fruit"

unicode(6) "orange"
unicode(1) "c"
unicode(5) "fruit"

unicode(6) "banana"
unicode(1) "d"
unicode(5) "fruit"

unicode(5) "apple"
unicode(1) "f"
unicode(5) "fruit"

bool(true)
-- Using array_walk_recursive with default parameters to show modified array contents --
unicode(5) "lemon"
unicode(1) "a"

unicode(6) "orange"
unicode(1) "c"

unicode(6) "banana"
unicode(1) "d"

unicode(5) "apple"
unicode(1) "f"

bool(true)
Done
