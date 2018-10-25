--TEST--
Test array_key_last() function
--FILE--
<?php

array_key_last($GLOBALS);

/* Various combinations of arrays to be used for the test */
$mixed_array = array(
  array(),
  array( 1,2,3,4,5,6,7,8,9 ),
  array( "One", "_Two", "Three", "Four", "Five" ),
  array( 6, "six", 7, "seven", 8, "eight", 9, "nine" ),
  array( "a" => "aaa", "A" => "AAA", "c" => "ccc", "d" => "ddd", "e" => "eee" ),
  array( "1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five" ),
  array( 1 => "one", 2 => "two", 3 => 7, 4 => "four", 5 => "five" ),
  array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2.4 => "float", "F" => "FFF",
         "blank" => "", 3.7 => 3.7, 5.4 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 ),
  array( 12, "name", 'age', '45' ),
  array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array() ),
  array( "one" => 1, "one" => 2, "three" => 3, 3, 4, 3 => 33, 4 => 44, 5, 6,
          5.4 => 54, 5.7 => 57, "5.4" => 554, "5.7" => 557 ),
  array( "foo" ),
  array( 1 => "42" )
);

/* Loop to test normal functionality with different arrays inputs */
echo "\n*** Normal testing with various array inputs ***\n";

$counter = 1;
foreach( $mixed_array as $sub_array )
{
 echo "\n-- Input Array for Iteration $counter is --\n";
 print_r( $sub_array );
 echo "\nLast key is :\n";
 var_dump( array_key_last($sub_array) );
 $counter++;
}

echo"\nDone";
?>
--EXPECT--
*** Normal testing with various array inputs ***

-- Input Array for Iteration 1 is --
Array
(
)

Last key is :
NULL

-- Input Array for Iteration 2 is --
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
)

Last key is :
int(8)

-- Input Array for Iteration 3 is --
Array
(
    [0] => One
    [1] => _Two
    [2] => Three
    [3] => Four
    [4] => Five
)

Last key is :
int(4)

-- Input Array for Iteration 4 is --
Array
(
    [0] => 6
    [1] => six
    [2] => 7
    [3] => seven
    [4] => 8
    [5] => eight
    [6] => 9
    [7] => nine
)

Last key is :
int(7)

-- Input Array for Iteration 5 is --
Array
(
    [a] => aaa
    [A] => AAA
    [c] => ccc
    [d] => ddd
    [e] => eee
)

Last key is :
string(1) "e"

-- Input Array for Iteration 6 is --
Array
(
    [1] => one
    [2] => two
    [3] => three
    [4] => four
    [5] => five
)

Last key is :
int(5)

-- Input Array for Iteration 7 is --
Array
(
    [1] => one
    [2] => two
    [3] => 7
    [4] => four
    [5] => five
)

Last key is :
int(5)

-- Input Array for Iteration 8 is --
Array
(
    [f] => fff
    [1] => one
    [4] => 6
    [] => 3
    [2] => float
    [F] => FFF
    [blank] => 
    [3] => 3.7
    [5] => Five
    [6] => 8.6
    [4name] => jonny
    [a] => 
)

Last key is :
string(1) "a"

-- Input Array for Iteration 9 is --
Array
(
    [0] => 12
    [1] => name
    [2] => age
    [3] => 45
)

Last key is :
int(3)

-- Input Array for Iteration 10 is --
Array
(
    [0] => Array
        (
            [0] => oNe
            [1] => tWo
            [2] => 4
        )

    [1] => Array
        (
            [0] => 10
            [1] => 20
            [2] => 30
            [3] => 40
            [4] => 50
        )

    [2] => Array
        (
        )

)

Last key is :
int(2)

-- Input Array for Iteration 11 is --
Array
(
    [one] => 2
    [three] => 3
    [0] => 3
    [1] => 4
    [3] => 33
    [4] => 44
    [5] => 57
    [6] => 6
    [5.4] => 554
    [5.7] => 557
)

Last key is :
string(3) "5.7"

-- Input Array for Iteration 12 is --
Array
(
    [0] => foo
)

Last key is :
int(0)

-- Input Array for Iteration 13 is --
Array
(
    [1] => 42
)

Last key is :
int(1)

Done
