--TEST--
Test array_key_first() function (variation)
--FILE--
<?php

/* Various combinations of arrays to be used for the test */
$array = array( 1,2,3,4,5,6,7,8,9 );

echo"\n*** Checking for internal array pointer not being changed by array_key_first ***\n";

echo "\nCurrent Element is : ";
var_dump( current($array) );

echo "\nNext Element is : ";
var_dump( next($array) );

echo "\nFirst key is : ";
var_dump( array_key_first($array) );

echo "\nCurrent Element after array_key_first operation is: ";
var_dump( current($array) );

echo"\nDone";
?>
--EXPECT--
*** Checking for internal array pointer not being changed by array_key_first ***

Current Element is : int(1)

Next Element is : int(2)

First key is : int(0)

Current Element after array_key_first operation is: int(2)

Done
