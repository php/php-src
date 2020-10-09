--TEST--
Test vfprintf() function : basic functionality - char format
--FILE--
<?php
echo "*** Testing vfprintf() : basic functionality - using char format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%c";
$format2 = "%c %c";
$format3 = "%c %c %c";
$arg1 = array(65);
$arg2 = array(65,66);
$arg3 = array(65,66,67);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic5.txt';
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
*** Testing vfprintf() : basic functionality - using char format ***
A
A B
A B C
