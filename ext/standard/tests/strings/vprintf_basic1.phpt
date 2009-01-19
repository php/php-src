--TEST--
Test vprintf() function : basic functionality - string format
--FILE--
<?php
/* Prototype  : int vprintf(string $format , array $args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vprintf() : basic functionality - using string format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%s";
$format2 = "%s %s";
$format3 = "%s %s %s";
$arg1 = array("one");
$arg2 = array("one","two");
$arg3 = array("one","two","three");


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
*** Testing vprintf() : basic functionality - using string format ***
one
int(3)
one two
int(7)
one two three
int(13)
===DONE===

