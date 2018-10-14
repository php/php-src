--TEST--
Test vprintf() function : basic functionality - hexadecimal format
--FILE--
<?php
/* Prototype  : string vprintf(string $format , array $args)
 * Description: Output a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vprintf() : basic functionality - using hexadecimal format ***\n";

// Initialising different format strings
$format = "format";
$format1 = "%x";
$format2 = "%x %x";
$format3 = "%x %x %x";

$format11 = "%X";
$format22 = "%X %X";
$format33 = "%X %X %X";

$arg1 = array(11);
$arg2 = array(11,132);
$arg3 = array(11,132,177);

$result = vprintf($format1,$arg1);
echo "\n";
var_dump($result);
$result = vprintf($format11,$arg1);
echo "\n";
var_dump($result);

$result = vprintf($format2,$arg2);
echo "\n";
var_dump($result);
$result = vprintf($format22,$arg2);
echo "\n";
var_dump($result);

$result = vprintf($format3,$arg3);echo "\n";
var_dump($result);
$result = vprintf($format33,$arg3);
echo "\n";
var_dump($result);

?>
===DONE===
--EXPECT--
*** Testing vprintf() : basic functionality - using hexadecimal format ***
b
int(1)
B
int(1)
b 84
int(4)
B 84
int(4)
b 84 b1
int(7)
B 84 B1
int(7)
===DONE===
