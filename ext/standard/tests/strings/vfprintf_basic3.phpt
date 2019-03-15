--TEST--
Test vfprintf() function : basic functionality - float format
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vfprintf() : basic functionality - using float format ***\n";

// Initialise all required variables

$format = "format";
$format1 = "%f";
$format2 = "%f %f";
$format3 = "%f %f %f";

$format11 = "%F";
$format22 = "%F %F";
$format33 = "%F %F %F";
$arg1 = array(11.11);
$arg2 = array(11.11,22.22);
$arg3 = array(11.11,22.22,33.33);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic3.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

vfprintf($fp, $format1,$arg1);
fprintf($fp, "\n");

vfprintf($fp,$format11,$arg1);
fprintf($fp, "\n");

vfprintf($fp,$format2,$arg2);
fprintf($fp, "\n");

vfprintf($fp,$format22,$arg2);
fprintf($fp, "\n");

vfprintf($fp,$format3,$arg3);
fprintf($fp, "\n");

vfprintf($fp, $format33,$arg3);
fprintf($fp, "\n");

fclose($fp);
print_r(file_get_contents($data_file));

unlink($data_file);
?>
===DONE===
--EXPECT--
*** Testing vfprintf() : basic functionality - using float format ***
11.110000
11.110000
11.110000 22.220000
11.110000 22.220000
11.110000 22.220000 33.330000
11.110000 22.220000 33.330000
===DONE===
