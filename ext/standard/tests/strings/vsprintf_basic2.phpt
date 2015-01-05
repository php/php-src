--TEST--
Test vsprintf() function : basic functionality - integer format
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , array $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 *  Testing vsprintf() : basic functionality - using integer format
*/

echo "*** Testing vsprintf() : basic functionality - using integer format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%d";
$format2 = "%d %d";
$format3 = "%d %d %d";
$arg1 = array(111);
$arg2 = array(111,222);
$arg3 = array(111,222,333);

var_dump( vsprintf($format1,$arg1) );
var_dump( vsprintf($format2,$arg2) );
var_dump( vsprintf($format3,$arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : basic functionality - using integer format ***
string(3) "111"
string(7) "111 222"
string(11) "111 222 333"
Done
