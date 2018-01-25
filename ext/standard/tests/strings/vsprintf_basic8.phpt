--TEST--
Test vsprintf() function : basic functionality - octal format
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , array $args)
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vsprintf() : basic functionality - using octal format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%o";
$format2 = "%o %o";
$format3 = "%o %o %o";
$arg1 = array(021);
$arg2 = array(021,0347);
$arg3 = array(021,0347,0567);

var_dump( vsprintf($format1,$arg1) );
var_dump( vsprintf($format2,$arg2) );
var_dump( vsprintf($format3,$arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : basic functionality - using octal format ***
string(2) "21"
string(6) "21 347"
string(10) "21 347 567"
Done
