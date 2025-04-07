--TEST--
Test array_walk_recursive() function : usage variations - 'input' array with subarray
--FILE--
<?php
/*
 * Testing array_walk_recursive() with an array having subarrays as elements
*/

echo "*** Testing array_walk_recursive() : array with subarray ***\n";

function callback($value, $key)
{
   // dump the arguments to check that they are passed
   // with proper type
   var_dump($key);  // key
   var_dump($value); // value
   echo "\n"; // new line to separate the output between each element
}

$input = array(
  array(),
  array(1),
  array(1,2,3),
  array("Mango", "Orange"),
  array(array(1, 2, 3), array(1))
);

var_dump( array_walk_recursive( $input, "callback"));

echo "Done"
?>
--EXPECT--
*** Testing array_walk_recursive() : array with subarray ***
int(0)
int(1)

int(0)
int(1)

int(1)
int(2)

int(2)
int(3)

int(0)
string(5) "Mango"

int(1)
string(6) "Orange"

int(0)
int(1)

int(1)
int(2)

int(2)
int(3)

int(0)
int(1)

bool(true)
Done
