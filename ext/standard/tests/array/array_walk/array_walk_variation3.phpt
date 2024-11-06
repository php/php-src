--TEST--
Test array_walk() function : usage variations - 'input' array with different values
--FILE--
<?php
/*
 * Testing array_walk() with following types of 'input' arrays:
 *    integer, float, string, bool, null, empty & mixed
*/

function print_value($value, $key, $count)
{
  echo  $count." : ".$key." ".$value."\n";
}

echo "*** Testing array_walk() : 'input' array with different values***\n";

// different arrays as input
$input_values = array(

       // integer values
/*1*/  array(1, 0, -10, 023, -041, 0x5A, 0X1F, -0x6E),

       // float value
       array(3.4, 0.8, -2.9, 6.25e2, 8.20E-3),

       // string values
       array('Mango', "Apple", 'Orange', "Lemon"),

       // bool values
/*4*/  array(true, false, TRUE, FALSE),

       // null values
       array(null, NULL),

       // empty array
       array(),

       // binary array
       array(b"binary"),

       // mixed array
/*8*/  array(16, 8.345, "Fruits", true, null, FALSE, -98, 0.005, 'banana')
);

for($count = 0; $count < count($input_values); $count++) {
  echo "\n-- Iteration ".($count + 1)." --\n";
  var_dump( array_walk($input_values[$count], "print_value", $count+1));
}
echo "Done"
?>
--EXPECT--
*** Testing array_walk() : 'input' array with different values***

-- Iteration 1 --
1 : 0 1
1 : 1 0
1 : 2 -10
1 : 3 19
1 : 4 -33
1 : 5 90
1 : 6 31
1 : 7 -110
bool(true)

-- Iteration 2 --
2 : 0 3.4
2 : 1 0.8
2 : 2 -2.9
2 : 3 625
2 : 4 0.0082
bool(true)

-- Iteration 3 --
3 : 0 Mango
3 : 1 Apple
3 : 2 Orange
3 : 3 Lemon
bool(true)

-- Iteration 4 --
4 : 0 1
4 : 1 
4 : 2 1
4 : 3 
bool(true)

-- Iteration 5 --
5 : 0 
5 : 1 
bool(true)

-- Iteration 6 --
bool(true)

-- Iteration 7 --
7 : 0 binary
bool(true)

-- Iteration 8 --
8 : 0 16
8 : 1 8.345
8 : 2 Fruits
8 : 3 1
8 : 4 
8 : 5 
8 : 6 -98
8 : 7 0.005
8 : 8 banana
bool(true)
Done
