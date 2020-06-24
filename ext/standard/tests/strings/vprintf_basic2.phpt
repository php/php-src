--TEST--
Test vprintf() function : basic functionality - integer format
--FILE--
<?php
/*
 *  Testing vprintf() : basic functionality - using integer format
*/

echo "*** Testing vprintf() : basic functionality - using integer format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%d";
$format2 = "%d %d";
$format3 = "%d %d %d";
$arg1 = array(111);
$arg2 = array(111,222);
$arg3 = array(111,222,333);

$result = vprintf($format1,$arg1);
echo "\n";
var_dump($result);

$result = vprintf($format2,$arg2);
echo "\n";
var_dump($result);

$result = vprintf($format3,$arg3);
echo "\n";
var_dump($result);

?>
--EXPECT--
*** Testing vprintf() : basic functionality - using integer format ***
111
int(3)
111 222
int(7)
111 222 333
int(11)
