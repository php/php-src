--TEST--
Test fgets() function : error conditions
--FILE--
<?php
/*
 Prototype: string fgets ( resource $handle [, int $length] );
 Description: Gets line from file pointer
*/

echo "*** Testing error conditions ***\n";

$fp = fopen(__FILE__, "r");

// invalid length argument
echo "-- Testing fgets() with invalid length arguments --\n";
$len = 0;
var_dump( fgets($fp, $len) );
$len = -10;
var_dump( fgets($fp, $len) );
$len = 1;
var_dump( fgets($fp, $len) ); // return length - 1 always, expect false

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
-- Testing fgets() with invalid length arguments --

Warning: fgets(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fgets(): Length parameter must be greater than 0 in %s on line %d
bool(false)
bool(false)
Done
