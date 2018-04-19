--TEST--
Test fprintf() function (variation - 7)
--SKIPIF--
<?php
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'w'))) {
  die('skip File dump.txt could not be created');
}
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

/* octal type variations */
fprintf($fp, "\n*** Testing fprintf() for octals ***\n");
foreach( $int_numbers as $octal_num ) {
 fprintf( $fp, "\n");
 fprintf( $fp, "%o", $octal_num );
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() for octals ***

0
1
1777777777777777777777
2
1777777777777777777776
131004725
1777777777777646773053
2322
Done
