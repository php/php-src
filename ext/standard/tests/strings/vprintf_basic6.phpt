--TEST--
Test vprintf() function : basic functionality - exponential format
--FILE--
<?php
/* Prototype  : string vprintf(string $format , array $args)
 * Description: Output a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vprintf() : basic functionality - using exponential format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%e";
$format2 = "%e %e";
$format3 = "%e %e %e";
$arg1 = array(1000);
$arg2 = array(1000,2000);
$arg3 = array(1000,2000,3000);

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
===DONE===
--EXPECT--
*** Testing vprintf() : basic functionality - using exponential format ***
1.000000e+3
int(11)
1.000000e+3 2.000000e+3
int(23)
1.000000e+3 2.000000e+3 3.000000e+3
int(35)
===DONE===
