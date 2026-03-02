--TEST--
Test fprintf() function (variation - 8)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

$int_variation = array( "%d", "%-d", "%+d", "%7.2d", "%-7.2d", "%07.2d", "%-07.2d", "%'#7.2d" );
$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = __DIR__ . '/fprintf_variation_008_64bit.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

/* hexadecimal type variations */
fprintf($fp, "\n*** Testing fprintf() for hexadecimals ***\n");
foreach( $int_numbers as $hexa_num ) {
 fprintf( $fp, "\n");
 fprintf( $fp, "%x", $hexa_num );
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() for hexadecimals ***

0
1
ffffffffffffffff
2
fffffffffffffffe
16409d5
fffffffffe9bf62b
4d2
Done
