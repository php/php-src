--TEST--
Test array_walk_recursive() function : usage variations - 'input' array with different values
--FILE--
<?php
/*
 * Testing array_walk_recursive() with following types of 'input' arrays:
 *    integer, float, string, bool, null, empty & mixed
*/

function print_value($value, $key, $count)
{
  echo  $count." : ".$key." ".$value."\n";
}

echo "*** Testing array_walk_recursive() : 'input' array with different values***\n";

// different arrays as input
$input_values = array(

       // integer values
/*1*/  array(array(1, 0, -10), array(023, -041), array(0x5A, 0X1F, -0x6E)),

       // float value
       array(array(3.4, 0.8, -2.9), array(6.25e2, 8.20E-3)),

       // string values
       array('Mango', array("Apple", 'Orange', "Lemon")),

       // bool values
/*4*/  array( array(true, false), array(TRUE, FALSE)),

       // null values
       array( array(null), array(NULL)),

       // empty array
       array(),

       // binary array
       array(array(b'binary')),

       // mixed array
/*8*/  array(16, 8.345, array("Fruits"), array(true, null), array(FALSE), array(-98, 0.005, 'banana'))
);

for($count = 0; $count < count($input_values); $count++) {
  echo "\n-- Iteration ".($count + 1)." --\n";
  var_dump( array_walk_recursive($input_values[$count], "print_value", $count+1));
}
echo "Done"
?>
--EXPECT--
*** Testing array_walk_recursive() : 'input' array with different values***

-- Iteration 1 --
1 : 0 1
1 : 1 0
1 : 2 -10
1 : 0 19
1 : 1 -33
1 : 0 90
1 : 1 31
1 : 2 -110
bool(true)

-- Iteration 2 --
2 : 0 3.4
2 : 1 0.8
2 : 2 -2.9
2 : 0 625
2 : 1 0.0082
bool(true)

-- Iteration 3 --
3 : 0 Mango
3 : 0 Apple
3 : 1 Orange
3 : 2 Lemon
bool(true)

-- Iteration 4 --
4 : 0 1
4 : 1 
4 : 0 1
4 : 1 
bool(true)

-- Iteration 5 --
5 : 0 
5 : 0 
bool(true)

-- Iteration 6 --
bool(true)

-- Iteration 7 --
7 : 0 binary
bool(true)

-- Iteration 8 --
8 : 0 16
8 : 1 8.345
8 : 0 Fruits
8 : 0 1
8 : 1 
8 : 0 
8 : 0 -98
8 : 1 0.005
8 : 2 banana
bool(true)
Done
