--TEST--
Test vfprintf() function : basic functionality - string format
--FILE--
<?php
echo "*** Testing vfprintf() : basic functionality - using string format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%s\n";
$format2 = "%s %s\n";
$format3 = "%s %s %s\n";
$arg1 = array("one");
$arg2 = array("one","two");
$arg3 = array("one","two","three");


/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_basic1.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

$result = vfprintf($fp, $format1, $arg1);
var_dump($result);
$result = vfprintf($fp, $format2, $arg2);
var_dump($result);
$result = vfprintf($fp, $format3, $arg3);
var_dump($result);

fclose($fp);
print_r(file_get_contents($data_file));

unlink($data_file);

?>
--EXPECT--
*** Testing vfprintf() : basic functionality - using string format ***
int(4)
int(8)
int(14)
one
one two
one two three
