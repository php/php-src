--TEST--
Test vfprintf() function : basic functionality - octal format
--FILE--
<?php
echo "*** Testing vfprintf() : basic functionality - using octal format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%o";
$format2 = "%o %o";
$format3 = "%o %o %o";
$arg1 = array(021);
$arg2 = array(021,0347);
$arg3 = array(021,0347,0567);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic8.txt';
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
*** Testing vfprintf() : basic functionality - using octal format ***
21
21 347
21 347 567
