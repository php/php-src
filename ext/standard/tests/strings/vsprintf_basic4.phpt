--TEST--
Test vsprintf() function : basic functionality - bool format
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , array $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vsprintf() : basic functionality - using bool format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%b";
$format2 = "%b %b";
$format3 = "%b %b %b";
$arg1 = array(TRUE);
$arg2 = array(TRUE,FALSE);
$arg3 = array(TRUE,FALSE,TRUE);

var_dump( vsprintf($format1,$arg1) );
var_dump( vsprintf($format2,$arg2) );
var_dump( vsprintf($format3,$arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : basic functionality - using bool format ***
string(1) "1"
string(3) "1 0"
string(5) "1 0 1"
Done
