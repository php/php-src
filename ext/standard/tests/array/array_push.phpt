--TEST--
Test array_push() function
--FILE--
<?php

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
  array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2 => "float", "F" => "FFF",
         "blank" => "", 3 => 3.7, 5 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 ),
  array( 12, "name", 'age', '45' ),
  array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array() ),
  array( "one" => 1, "one" => 2, "three" => 3, 3, 4, 3 => 33, 4 => 44, 5, 6,
          5 => 57, "5.4" => 554, "5.7" => 557 )
);

/* Error Conditions */
echo "\n*** Testing Edge Conditions ***\n";

/* Invalid Number of arguments */
var_dump( array_push($mixed_array[1],1,2) );

/* Empty Array as argument */
var_dump( array_push($empty_array, 2) );


/* Loop to test normal functionality with different arrays inputs */
echo "\n*** Testing with various array inputs ***\n";

$counter = 1;
foreach( $mixed_array as $sub_array )
{
 echo "\n-- Input Array for Iteration $counter is --\n";
 var_dump( $sub_array );
 echo "\nOutput after push is :\n";
 var_dump( array_push($sub_array, 22, "abc") );
 $counter++;
}

/* Checking for return value and the new array formed from push operation */
echo "\n*** Checking for return value and the new array formed from push operation ***\n";
var_dump( array_push($mixed_array[2], 22, 33, "44") );
var_dump( $mixed_array[2] );

echo"\nDone";
?>
--EXPECT--
*** Testing Edge Conditions ***
int(11)
int(1)

*** Testing with various array inputs ***

-- Input Array for Iteration 1 is --
array(0) {
}

Output after push is :
int(2)

-- Input Array for Iteration 2 is --
array(11) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(1)
  [10]=>
  int(2)
}

Output after push is :
int(13)

-- Input Array for Iteration 3 is --
array(5) {
  [0]=>
  string(3) "One"
  [1]=>
  string(4) "_Two"
  [2]=>
  string(5) "Three"
  [3]=>
  string(4) "Four"
  [4]=>
  string(4) "Five"
}

Output after push is :
int(7)

-- Input Array for Iteration 4 is --
array(8) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
  [4]=>
  int(8)
  [5]=>
  string(5) "eight"
  [6]=>
  int(9)
  [7]=>
  string(4) "nine"
}

Output after push is :
int(10)

-- Input Array for Iteration 5 is --
array(5) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}

Output after push is :
int(7)

-- Input Array for Iteration 6 is --
array(5) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  [4]=>
  string(4) "four"
  [5]=>
  string(4) "five"
}

Output after push is :
int(7)

-- Input Array for Iteration 7 is --
array(5) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  int(7)
  [4]=>
  string(4) "four"
  [5]=>
  string(4) "five"
}

Output after push is :
int(7)

-- Input Array for Iteration 8 is --
array(12) {
  ["f"]=>
  string(3) "fff"
  [1]=>
  string(3) "one"
  [4]=>
  int(6)
  [""]=>
  int(3)
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
  [3]=>
  float(3.7)
  [5]=>
  string(4) "Five"
  [6]=>
  float(8.6)
  ["4name"]=>
  string(5) "jonny"
  ["a"]=>
  NULL
}

Output after push is :
int(14)

-- Input Array for Iteration 9 is --
array(4) {
  [0]=>
  int(12)
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
  [3]=>
  string(2) "45"
}

Output after push is :
int(6)

-- Input Array for Iteration 10 is --
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "oNe"
    [1]=>
    string(3) "tWo"
    [2]=>
    int(4)
  }
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}

Output after push is :
int(5)

-- Input Array for Iteration 11 is --
array(10) {
  ["one"]=>
  int(2)
  ["three"]=>
  int(3)
  [0]=>
  int(3)
  [1]=>
  int(4)
  [3]=>
  int(33)
  [4]=>
  int(44)
  [5]=>
  int(57)
  [6]=>
  int(6)
  ["5.4"]=>
  int(554)
  ["5.7"]=>
  int(557)
}

Output after push is :
int(12)

*** Checking for return value and the new array formed from push operation ***
int(8)
array(8) {
  [0]=>
  string(3) "One"
  [1]=>
  string(4) "_Two"
  [2]=>
  string(5) "Three"
  [3]=>
  string(4) "Four"
  [4]=>
  string(4) "Five"
  [5]=>
  int(22)
  [6]=>
  int(33)
  [7]=>
  string(2) "44"
}

Done
