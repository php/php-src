--TEST--
Test vprintf() function : basic functionality - octal format
--FILE--
<?php
echo "*** Testing vprintf() : basic functionality - using octal format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%o";
$format2 = "%o %o";
$format3 = "%o %o %o";
$arg1 = array(021);
$arg2 = array(021,0347);
$arg3 = array(021,0347,0567);

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
*** Testing vprintf() : basic functionality - using octal format ***
21
int(2)
21 347
int(6)
21 347 567
int(10)
