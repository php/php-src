--TEST--
Test array_pop() function
--FILE--
<?php

array_pop($GLOBALS);

$empty_array = array();
$number = 5;
$str = "abc";


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
          5.4 => 54, 5.7 => 57, "5.4" => 554, "5.7" => 557 )
);

/* Loop to test normal functionality with different arrays inputs */
echo "\n*** Normal testing with various array inputs ***\n";

$counter = 1;
foreach( $mixed_array as $sub_array )
{
 echo "\n-- Input Array for Iteration $counter is --\n";
 print_r( $sub_array );
 echo "\nOutput after Pop is :\n";
 var_dump( array_pop($sub_array) );
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

Output after Pop is :
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

Output after Pop is :
int(9)

-- Input Array for Iteration 3 is --
Array
(
    [0] => One
    [1] => _Two
    [2] => Three
    [3] => Four
    [4] => Five
)

Output after Pop is :
string(4) "Five"

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

Output after Pop is :
string(4) "nine"

-- Input Array for Iteration 5 is --
Array
(
    [a] => aaa
    [A] => AAA
    [c] => ccc
    [d] => ddd
    [e] => eee
)

Output after Pop is :
string(3) "eee"

-- Input Array for Iteration 6 is --
Array
(
    [1] => one
    [2] => two
    [3] => three
    [4] => four
    [5] => five
)

Output after Pop is :
string(4) "five"

-- Input Array for Iteration 7 is --
Array
(
    [1] => one
    [2] => two
    [3] => 7
    [4] => four
    [5] => five
)

Output after Pop is :
string(4) "five"

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

Output after Pop is :
NULL

-- Input Array for Iteration 9 is --
Array
(
    [0] => 12
    [1] => name
    [2] => age
    [3] => 45
)

Output after Pop is :
string(2) "45"

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

Output after Pop is :
array(0) {
}

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

Output after Pop is :
int(557)

Done
