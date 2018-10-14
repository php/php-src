--TEST--
Test array_shift() function
--FILE--
<?php
/* Prototype: mixed array_shift( array &array );
 * Description: Shifts the first value of the array off and returns it.
 */

array_shift($GLOBALS);

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

/* Testing Error Conditions */
echo "\n*** Testing Error Conditions ***\n";

/* Zero argument  */
var_dump( array_shift() );

/* Scalar argument */
var_dump( array_shift($number) );

/* String argument */
var_dump( array_shift($str) );

/* Invalid Number of arguments */
var_dump( array_shift($mixed_array[1],$mixed_array[2]) );

/* Empty Array as argument */
var_dump( array_shift($empty_array) );

/* Loop to test normal functionality with different arrays inputs */
echo "\n*** Testing with various array inputs ***\n";

$counter = 1;
foreach( $mixed_array as $sub_array ) {
  echo "\n-- Input Array for Iteration $counter is -- \n";
  print_r( $sub_array );
  echo "\nOutput after shift is :\n";
  var_dump( array_shift($sub_array) );
  $counter++;
}

/*Checking for internal array pointer beint reset when shift is called */

echo"\n*** Checking for internal array pointer being reset when shift is called ***\n";

echo "\nCurrent Element is : ";
var_dump( current($mixed_array[1]) );

echo "\nNext Element is : ";
var_dump( next($mixed_array[1]) );

echo "\nNext Element is : ";
var_dump( next($mixed_array[1]) );

echo "\nshifted Element is : ";
var_dump( array_shift($mixed_array[1]) );

echo "\nCurrent Element after shift operation is: ";
var_dump( current($mixed_array[1]) );

echo"Done";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: array_shift() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: array_shift() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: array_shift() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_shift() expects exactly 1 parameter, 2 given in %s on line %d
NULL
NULL

*** Testing with various array inputs ***

-- Input Array for Iteration 1 is -- 
Array
(
)

Output after shift is :
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

Output after shift is :
int(1)

-- Input Array for Iteration 3 is -- 
Array
(
    [0] => One
    [1] => _Two
    [2] => Three
    [3] => Four
    [4] => Five
)

Output after shift is :
string(3) "One"

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

Output after shift is :
int(6)

-- Input Array for Iteration 5 is -- 
Array
(
    [a] => aaa
    [A] => AAA
    [c] => ccc
    [d] => ddd
    [e] => eee
)

Output after shift is :
string(3) "aaa"

-- Input Array for Iteration 6 is -- 
Array
(
    [1] => one
    [2] => two
    [3] => three
    [4] => four
    [5] => five
)

Output after shift is :
string(3) "one"

-- Input Array for Iteration 7 is -- 
Array
(
    [1] => one
    [2] => two
    [3] => 7
    [4] => four
    [5] => five
)

Output after shift is :
string(3) "one"

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

Output after shift is :
string(3) "fff"

-- Input Array for Iteration 9 is -- 
Array
(
    [0] => 12
    [1] => name
    [2] => age
    [3] => 45
)

Output after shift is :
int(12)

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

Output after shift is :
array(3) {
  [0]=>
  string(3) "oNe"
  [1]=>
  string(3) "tWo"
  [2]=>
  int(4)
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

Output after shift is :
int(2)

*** Checking for internal array pointer being reset when shift is called ***

Current Element is : int(1)

Next Element is : int(2)

Next Element is : int(3)

shifted Element is : int(1)

Current Element after shift operation is: int(2)
Done
