--TEST--
Test array_walk() function : basic functionality - associative array
--FILE--
<?php
echo "*** Testing array_walk() : basic functionality ***\n";

// associative array
$fruits = array("d" => "lemon", "a" => "orange", "b" => "banana", "c" => "apple");

function test_alter(&$item, $key, $prefix)
{
  // dump the arguments to check that they are passed
  // with proper type
  var_dump($item); // value
  var_dump($key);  // key
  var_dump($prefix); // additional argument passed to callback function
  echo "\n"; // new line to separate the output between each element
}

function test_print($item, $key)
{
  // dump the arguments to check that they are passed
  // with proper type
  var_dump($item); // value
  var_dump($key);  // key
  echo "\n"; // new line to separate the output between each element
}

echo "-- Using array_walk with default parameters to show array contents --\n";
var_dump(array_walk($fruits, 'test_print'));

echo "-- Using array_walk with one optional parameter to modify contents --\n";
var_dump (array_walk($fruits, 'test_alter', 'fruit'));

echo "-- Using array_walk with default parameters to show modified array contents --\n";
var_dump (array_walk($fruits, 'test_print'));

echo "Done";
?>
--EXPECT--
*** Testing array_walk() : basic functionality ***
-- Using array_walk with default parameters to show array contents --
string(5) "lemon"
string(1) "d"

string(6) "orange"
string(1) "a"

string(6) "banana"
string(1) "b"

string(5) "apple"
string(1) "c"

bool(true)
-- Using array_walk with one optional parameter to modify contents --
string(5) "lemon"
string(1) "d"
string(5) "fruit"

string(6) "orange"
string(1) "a"
string(5) "fruit"

string(6) "banana"
string(1) "b"
string(5) "fruit"

string(5) "apple"
string(1) "c"
string(5) "fruit"

bool(true)
-- Using array_walk with default parameters to show modified array contents --
string(5) "lemon"
string(1) "d"

string(6) "orange"
string(1) "a"

string(6) "banana"
string(1) "b"

string(5) "apple"
string(1) "c"

bool(true)
Done
