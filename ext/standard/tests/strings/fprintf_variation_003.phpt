--TEST--
Test fprintf() function (variation - 3)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = __DIR__ . '/fprintf_variation_003.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

/* binary type variations */
fprintf($fp, "\n*** Testing fprintf() with binary ***\n");
foreach( $int_numbers as $bin_num ) {
  fprintf( $fp, "\n");
  fprintf( $fp, "%b", $bin_num );
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() with binary ***

0
1
11111111111111111111111111111111
10
11111111111111111111111111111110
1011001000000100111010101
11111110100110111111011000101011
10011010010
Done
