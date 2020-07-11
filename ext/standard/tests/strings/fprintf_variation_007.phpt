--TEST--
Test fprintf() function (variation - 7)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = __DIR__ . '/fprintf_variation_007.txt';
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
37777777777
2
37777777776
131004725
37646773053
2322
Done
