--TEST--
Test fprintf() function (variation - 6)
--SKIPIF--
<?php
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'w'))) {
  die('skip File dump.txt could not be created');
}
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

/* unsigned int type variation */
fprintf($fp, "\n*** Testing fprintf() for unsigned integers ***\n");
foreach( $int_numbers as $unsig_num ) {
  fprintf( $fp, "\n");
  fprintf( $fp, "%u", $unsig_num );
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() for unsigned integers ***

0
1
4294967295
2
4294967294
23333333
4271633963
1234
Done
