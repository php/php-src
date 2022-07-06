--TEST--
Test array_key_last() function (variation)
--FILE--
<?php

/* Various combinations of arrays to be used for the test */
$array = array( 1,2,3,4,5,6,7,8,9 );

echo"\n*** Checking for internal array pointer not being changed by array_key_last ***\n";

echo "\nCurrent Element is : ";
var_dump( current($array) );

echo "\nNext Element is : ";
var_dump( next($array) );

echo "\nLast key is : ";
var_dump( array_key_last($array) );

echo "\nCurrent Element after array_key_last operation is: ";
var_dump( current($array) );

echo"\nDone";
?>
--EXPECT--
*** Checking for internal array pointer not being changed by array_key_last ***

Current Element is : int(1)

Next Element is : int(2)

Last key is : int(8)

Current Element after array_key_last operation is: int(2)

Done
