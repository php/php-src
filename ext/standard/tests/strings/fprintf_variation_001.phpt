--TEST--
Test fprintf() function (variation - 1)
--SKIPIF--
<?php
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'w'))) {
  die('skip File dump.txt could not be created');
}
?>
--FILE--
<?php

$float_variation = array( "%f","%-f", "%+f", "%7.2f", "%-7.2f", "%07.2f", "%-07.2f", "%'#7.2f" );
$float_numbers = array( 0, 1, -1, 0.32, -0.32, 3.4. -3.4, 2.54, -2.54 );

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

$counter = 1;
/* float type variations */
fprintf($fp, "\n*** Testing fprintf() with floats ***\n");

foreach( $float_variation as $float_var ) {
  fprintf( $fp, "\n-- Iteration %d --\n",$counter);
  foreach( $float_numbers as $float_num ) {
    fprintf( $fp, "\n");
    fprintf( $fp, $float_var, $float_num );
  }
  $counter++;
}

fclose($fp);
print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() with floats ***

-- Iteration 1 --

0.000000
1.000000
-1.000000
0.320000
-0.320000
3.400000
2.540000
-2.540000
-- Iteration 2 --

0.000000
1.000000
-1.000000
0.320000
-0.320000
3.400000
2.540000
-2.540000
-- Iteration 3 --

+0.000000
+1.000000
-1.000000
+0.320000
-0.320000
+3.400000
+2.540000
-2.540000
-- Iteration 4 --

   0.00
   1.00
  -1.00
   0.32
  -0.32
   3.40
   2.54
  -2.54
-- Iteration 5 --

0.00   
1.00   
-1.00  
0.32   
-0.32  
3.40   
2.54   
-2.54  
-- Iteration 6 --

0000.00
0001.00
-001.00
0000.32
-000.32
0003.40
0002.54
-002.54
-- Iteration 7 --

0.00000
1.00000
-1.0000
0.32000
-0.3200
3.40000
2.54000
-2.5400
-- Iteration 8 --

###0.00
###1.00
##-1.00
###0.32
##-0.32
###3.40
###2.54
##-2.54
Done
