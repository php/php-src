--TEST--
Test array_walk() function : usage variations - anonymous callback function
--FILE--
<?php
/*
* Passing anonymous(run-time) callback function with following variations:
*   with one parameter
*   two parameters
*   three parameters
*   extra parameters
*   without parameters
*/

echo "*** Testing array_walk() : anonymous function as callback ***\n";

$input = array(2, 5, 10, 0);

echo "-- Anonymous function with one argument --\n";
var_dump( array_walk($input, function($value) { var_dump($value); echo "\n"; }));

echo "-- Anonymous function with two arguments --\n";
var_dump( array_walk($input, function($value, $key) { var_dump($key); var_dump($value); echo "\n"; }));

echo "-- Anonymous function with three arguments --\n";
var_dump( array_walk($input, function($value, $key, $user_data) { var_dump($key); var_dump($value); var_dump($user_data); echo "\n"; }, 10));

echo "-- Anonymous function with null argument --\n";
var_dump( array_walk( $input, function() { echo "1\n"; }));
echo "Done"
?>
--EXPECT--
*** Testing array_walk() : anonymous function as callback ***
-- Anonymous function with one argument --
int(2)

int(5)

int(10)

int(0)

bool(true)
-- Anonymous function with two arguments --
int(0)
int(2)

int(1)
int(5)

int(2)
int(10)

int(3)
int(0)

bool(true)
-- Anonymous function with three arguments --
int(0)
int(2)
int(10)

int(1)
int(5)
int(10)

int(2)
int(10)
int(10)

int(3)
int(0)
int(10)

bool(true)
-- Anonymous function with null argument --
1
1
1
1
bool(true)
Done
