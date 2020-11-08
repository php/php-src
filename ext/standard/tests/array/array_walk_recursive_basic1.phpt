--TEST--
Test array_walk_recursive() function : basic functionality - regular array
--FILE--
<?php
echo "*** Testing array_walk_recursive() : basic functionality ***\n";

// regular array
$fruits = array("lemon", array("orange", "banana"), array("apple"));

function test_print($item, $key)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($item); // value
   var_dump($key);  // key
   echo "\n"; // new line to separate the output between each element
}
function with_userdata($item, $key, $user_data)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($item); // value
   var_dump($key);  // key
   var_dump($user_data); // user supplied data
   echo "\n"; // new line to separate the output between each element
}

echo "-- Using array_walk_recursive() with default parameters to show array contents --\n";
var_dump( array_walk_recursive($fruits, 'test_print'));

echo "-- Using array_walk_recursive() with all parameters --\n";
var_dump( array_walk_recursive($fruits, 'with_userdata', "Added"));

echo "Done";
?>
--EXPECT--
*** Testing array_walk_recursive() : basic functionality ***
-- Using array_walk_recursive() with default parameters to show array contents --
string(5) "lemon"
int(0)

string(6) "orange"
int(0)

string(6) "banana"
int(1)

string(5) "apple"
int(0)

bool(true)
-- Using array_walk_recursive() with all parameters --
string(5) "lemon"
int(0)
string(5) "Added"

string(6) "orange"
int(0)
string(5) "Added"

string(6) "banana"
int(1)
string(5) "Added"

string(5) "apple"
int(0)
string(5) "Added"

bool(true)
Done
