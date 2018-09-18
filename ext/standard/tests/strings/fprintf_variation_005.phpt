--TEST--
Test fprintf() function (variation - 5)
--SKIPIF--
<?php
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'w'))) {
  die('skip File dump.txt could not be created');
}
?>
--FILE--
<?php

$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

/* %e type variations */
fprintf($fp, "\n*** Testing fprintf() for scientific type ***\n");
foreach( $int_numbers as $num ) {
  fprintf( $fp, "\n");
  fprintf( $fp, "%e", $num );
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() for scientific type ***

0.000000e+0
1.000000e+0
-1.000000e+0
2.700000e+0
-2.700000e+0
2.333333e+7
-2.333333e+7
1.234000e+3
Done
