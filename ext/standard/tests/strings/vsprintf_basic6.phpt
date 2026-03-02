--TEST--
Test vsprintf() function : basic functionality - exponential format
--FILE--
<?php
echo "*** Testing vsprintf() : basic functionality - using exponential format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%e";
$format2 = "%e %e";
$format3 = "%e %e %e";
$arg1 = array(1000);
$arg2 = array(1000,2000);
$arg3 = array(1000,2000,3000);

var_dump( vsprintf($format1,$arg1) );
var_dump( vsprintf($format2,$arg2) );
var_dump( vsprintf($format3,$arg3) );

echo "Done";
?>
--EXPECT--
*** Testing vsprintf() : basic functionality - using exponential format ***
string(11) "1.000000e+3"
string(23) "1.000000e+3 2.000000e+3"
string(35) "1.000000e+3 2.000000e+3 3.000000e+3"
Done
