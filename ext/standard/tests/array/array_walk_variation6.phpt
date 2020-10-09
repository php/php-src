--TEST--
Test array_walk() function : usage variations - 'input' argument as diff. associative arrays
--FILE--
<?php
/*
 * Passing 'input' argument as an associative array
 *    with Numeric & string keys
*/

echo "*** Testing array_walk() : 'input' as an associative array ***\n";

function for_numeric($value, $key, $user_data)
{
  // dump the input values to see if they are
  // passed with correct type
  var_dump($key);
  var_dump($value);
  var_dump($user_data);
  echo "\n"; // new line to separate the output between each element
}

function for_string($value, $key)
{
  // dump the input values to see if they are
  // passed with correct type
  var_dump($key);
  var_dump($value);
  echo "\n"; // new line to separate the output between each element
}

function for_mixed($value, $key)
{
  // dump the input values to see if they are
  // passed with correct type
  var_dump($key);
  var_dump($value);
  echo "\n"; // new line to separate the output between each element
}

// Numeric keys
$input = array( 1 => 25, 5 => 12, 0 => -80, -2 => 100, 5 => 30);
echo "-- Associative array with numeric keys --\n";
var_dump( array_walk($input, "for_numeric", 10));

// String keys
$input = array( "a" => "Apple", 'b' => 'Bananna', "c" => "carrot", 'o' => "Orange");
echo "-- Associative array with string keys --\n";
var_dump( array_walk($input, "for_string"));

// binary keys
$input = array( b"a" => "Apple", b"b" => "Banana");
echo "-- Associative array with binary keys --\n";
var_dump( array_walk($input, "for_string"));

// Mixed keys - numeric/string
$input = array( 0 => 1, 1 => 2, "a" => "Apple", "b" => "Banana", 2 =>3);
echo "-- Associative array with numeric/string keys --\n";
var_dump( array_walk($input, "for_mixed"));

echo "Done"
?>
--EXPECT--
*** Testing array_walk() : 'input' as an associative array ***
-- Associative array with numeric keys --
int(1)
int(25)
int(10)

int(5)
int(30)
int(10)

int(0)
int(-80)
int(10)

int(-2)
int(100)
int(10)

bool(true)
-- Associative array with string keys --
string(1) "a"
string(5) "Apple"

string(1) "b"
string(7) "Bananna"

string(1) "c"
string(6) "carrot"

string(1) "o"
string(6) "Orange"

bool(true)
-- Associative array with binary keys --
string(1) "a"
string(5) "Apple"

string(1) "b"
string(6) "Banana"

bool(true)
-- Associative array with numeric/string keys --
int(0)
int(1)

int(1)
int(2)

string(1) "a"
string(5) "Apple"

string(1) "b"
string(6) "Banana"

int(2)
int(3)

bool(true)
Done
