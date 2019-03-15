--TEST--
Test vfprintf) function : basic functionality - hexadecimal format
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vfprintf) : basic functionality - using hexadecimal format ***\n";

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

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic9.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

vfprintf($fp, $format1, $arg1);
fprintf($fp, "\n");
vfprintf($fp, $format11, $arg1);
fprintf($fp, "\n");

vfprintf($fp, $format2, $arg2);
fprintf($fp, "\n");
vfprintf($fp, $format22, $arg2);
fprintf($fp, "\n");

vfprintf($fp, $format3, $arg3);
fprintf($fp, "\n");
vfprintf($fp, $format33, $arg3);
fprintf($fp, "\n");

fclose($fp);
print_r(file_get_contents($data_file));

unlink($data_file);
?>
===DONE===
--EXPECT--
*** Testing vfprintf) : basic functionality - using hexadecimal format ***
b
B
b 84
B 84
b 84 b1
B 84 B1
===DONE===
