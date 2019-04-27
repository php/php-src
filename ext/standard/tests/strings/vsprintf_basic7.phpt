--TEST--
Test vsprintf() function : basic functionality - unsigned format
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , array $args)
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
--EXPECT--
*** Testing vsprintf() : basic functionality - using unsigned format ***
string(10) "4294966185"
string(21) "4294966185 4293732729"
string(32) "4294966185 4293732729 4292621864"
Done
