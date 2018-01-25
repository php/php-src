--TEST--
Test array_walk() function : basic functionality - regular array
--FILE--
<?php
/* Prototype  : bool array_walk(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_walk() : basic functionality ***\n";

// regular array
$fruits = array("lemon", "orange", "banana", "apple");

/*  Prototype : test_print(mixed $item, mixed $key)
 *  Parameters : item - item in key/item pair
 *               key - key in key/item pair
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
function with_userdata($item, $key, $user_data)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($item); // value
   var_dump($key);  // key
   var_dump($user_data); // user supplied data
   echo "\n"; // new line to separate the output between each element
}

echo "-- Using array_walk() with default parameters to show array contents --\n";
var_dump( array_walk($fruits, 'test_print'));

echo "-- Using array_walk() with all parameters --\n";
var_dump( array_walk($fruits, 'with_userdata', "Added"));

echo "Done";
?>
--EXPECT--
*** Testing array_walk() : basic functionality ***
-- Using array_walk() with default parameters to show array contents --
string(5) "lemon"
int(0)

string(6) "orange"
int(1)

string(6) "banana"
int(2)

string(5) "apple"
int(3)

bool(true)
-- Using array_walk() with all parameters --
string(5) "lemon"
int(0)
string(5) "Added"

string(6) "orange"
int(1)
string(5) "Added"

string(6) "banana"
int(2)
string(5) "Added"

string(5) "apple"
int(3)
string(5) "Added"

bool(true)
Done
