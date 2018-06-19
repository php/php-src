--TEST--
Test array_value_first() function (variation)
--FILE--
<?php

/* Various combinations of arrays to be used for the test */
$array = array( 1,2,3,4,5,6,7,8,9 );

echo"\n*** Checking for internal array pointer not being changed by array_value_first ***\n";

echo "\nCurrent Element is : ";
var_dump( current($array) );

echo "\nNext Element is : ";
var_dump( next($array) );

echo "\nFirst value is : ";
var_dump( array_value_first($array) );

echo "\nCurrent Element after array_value_first operation is: ";
var_dump( current($array) );

echo"\nDone";
?>
--EXPECT--
*** Checking for internal array pointer not being changed by array_value_first ***

Current Element is : int(1)

Next Element is : int(2)

First value is : int(1)

Current Element after array_value_first operation is: int(2)

Done
