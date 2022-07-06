--TEST--
Test array_pop() function (variation)
--FILE--
<?php

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

echo"\n*** Checking for internal array pointer being reset when pop is called ***\n";

echo "\nCurrent Element is : ";
var_dump( current($mixed_array[1]) );

echo "\nNext Element is : ";
var_dump( next($mixed_array[1]) );

echo "\nNext Element is : ";
var_dump( next($mixed_array[1]) );

echo "\nPOPed Element is : ";
var_dump( array_pop($mixed_array[1]) );

echo "\nCurrent Element after POP operation is: ";
var_dump( current($mixed_array[1]) );

echo"\nDone";
?>
--EXPECT--
*** Checking for internal array pointer being reset when pop is called ***

Current Element is : int(1)

Next Element is : int(2)

Next Element is : int(3)

POPed Element is : int(9)

Current Element after POP operation is: int(1)

Done
