--TEST--
Test vfprintf() function : basic functionality - integer format
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 *  Testing vfprintf() : basic functionality - using integer format
*/

echo "*** Testing vfprintf() : basic functionality - using integer format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%d";
$format2 = "%d %d";
$format3 = "%d %d %d";
$arg1 = array(111);
$arg2 = array(111,222);
$arg3 = array(111,222,333);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic2.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

vfprintf($fp, $format1, $arg1);
fprintf($fp, "\n");

vfprintf($fp, $format2, $arg2);
fprintf($fp, "\n");

vfprintf($fp, $format3, $arg3);
fprintf($fp, "\n");

fclose($fp);
print_r(file_get_contents($data_file));

unlink($data_file);

?>
===DONE===
--EXPECT--
*** Testing vfprintf() : basic functionality - using integer format ***
111
111 222
111 222 333
===DONE===
