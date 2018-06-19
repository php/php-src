--TEST--
Test array_value_last() function (variation)
--FILE--
<?php

/* Various combinations of arrays to be used for the test */
$array = array( 1,2,3,4,5,6,7,8,9 );

echo"\n*** Checking for internal array pointer not being changed by array_value_last ***\n";

echo "\nCurrent Element is : ";
var_dump( current($array) );

echo "\nNext Element is : ";
var_dump( next($array) );

echo "\nLast value is : ";
var_dump( array_value_last($array) );

echo "\nCurrent Element after array_value_last operation is: ";
var_dump( current($array) );

echo"\nDone";
?>
--EXPECT--
*** Checking for internal array pointer not being changed by array_value_last ***

Current Element is : int(1)

Next Element is : int(2)

Last value is : int(9)

Current Element after array_value_last operation is: int(2)

Done
