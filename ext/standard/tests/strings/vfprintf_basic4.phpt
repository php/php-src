--TEST--
Test vfprintf() function : basic functionality - bool format
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vfprintf() : basic functionality - using bool format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%b";
$format2 = "%b %b";
$format3 = "%b %b %b";
$arg1 = array(TRUE);
$arg2 = array(TRUE,FALSE);
$arg3 = array(TRUE,FALSE,TRUE);

/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_basic4.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

vfprintf($fp, $format1,$arg1);
fprintf($fp, "\n");

vfprintf($fp, $format2,$arg2);
fprintf($fp, "\n");

vfprintf($fp, $format3,$arg3);
fprintf($fp, "\n");

fclose($fp);
print_r(file_get_contents($data_file));

unlink($data_file);
?>
===DONE===
--EXPECT--
*** Testing vfprintf() : basic functionality - using bool format ***
1
1 0
1 0 1
===DONE===

