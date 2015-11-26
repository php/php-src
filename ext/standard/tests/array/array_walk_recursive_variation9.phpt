--TEST--
Test array_walk_recursive() function : usage variations - different callback functions
--FILE--
<?php
/* Prototype  : bool array_walk_recursive(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

/*
 * Passing different types of callback functions to array_walk_recursive()
 *   without parameters
 *   with less and more parameters
*/

echo "*** Testing array_walk_recursive() : callback function variation ***\n";

$input = array(array('Apple', 'Banana'), 'Mango', array('Orange'));

echo "-- callback function with both parameters --\n";
function callback_two_parameter($value, $key)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($key);  // key
   var_dump($value); // value
   echo "\n"; // new line to separate the output between each element
}
var_dump( array_walk_recursive($input, 'callback_two_parameter'));

echo "-- callback function with only one parameter --\n";
function callback_one_parameter($value)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($value); // value
   echo "\n"; // new line to separate the output between each element
}
var_dump( array_walk_recursive($input, 'callback_one_parameter'));

echo "-- callback function without parameters --\n";
function callback_no_parameter()
{
  echo "callback3() called\n";
}
var_dump( array_walk_recursive($input, 'callback_no_parameter'));

echo "-- passing one more parameter to function with two parameters --\n";
var_dump( array_walk_recursive($input, 'callback_two_parameter', 10)); 

echo "Done"
?>
--EXPECTF--
*** Testing array_walk_recursive() : callback function variation ***
-- callback function with both parameters --
int(0)
string(5) "Apple"

int(1)
string(6) "Banana"

int(1)
string(5) "Mango"

int(0)
string(6) "Orange"

bool(true)
-- callback function with only one parameter --
string(5) "Apple"

string(6) "Banana"

string(5) "Mango"

string(6) "Orange"

bool(true)
-- callback function without parameters --
callback3() called
callback3() called
callback3() called
callback3() called
bool(true)
-- passing one more parameter to function with two parameters --
int(0)
string(5) "Apple"

int(1)
string(6) "Banana"

int(1)
string(5) "Mango"

int(0)
string(6) "Orange"

bool(true)
Done
