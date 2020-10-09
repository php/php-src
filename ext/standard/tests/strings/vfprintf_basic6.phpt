--TEST--
Test vfprintf() function : basic functionality - exponential format
--FILE--
<?php
echo "*** Testing vfprintf() : basic functionality - using exponential format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%e";
$format2 = "%e %e";
$format3 = "%e %e %e";
$arg1 = array(1000);
$arg2 = array(1000,2000);
$arg3 = array(1000,2000,3000);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic6.txt';
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
--EXPECT--
*** Testing vfprintf() : basic functionality - using exponential format ***
1.000000e+3
1.000000e+3 2.000000e+3
1.000000e+3 2.000000e+3 3.000000e+3
