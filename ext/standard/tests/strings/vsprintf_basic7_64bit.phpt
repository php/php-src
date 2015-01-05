--TEST--
Test vsprintf() function : basic functionality - unsigned format
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , aaray $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vsprintf() : basic functionality - using unsigned format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%u";
$format2 = "%u %u";
$format3 = "%u %u %u";
$arg1 = array(-1111);
$arg2 = array(-1111,-1234567);
$arg3 = array(-1111,-1234567,-2345432);

var_dump( vsprintf($format1,$arg1) );
var_dump( vsprintf($format2,$arg2) );
var_dump( vsprintf($format3,$arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : basic functionality - using unsigned format ***
string(20) "18446744073709550505"
string(41) "18446744073709550505 18446744073708317049"
string(62) "18446744073709550505 18446744073708317049 18446744073707206184"
Done
