--TEST--
Test fprintf() function (variation - 2)
--SKIPIF--
<?php
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'w'))) {
  die('skip File dump.txt could not be created');
}
?>
--FILE--
<?php

$int_variation = array( "%d", "%-d", "%+d", "%7.2d", "%-7.2d", "%07.2d", "%-07.2d", "%'#7.2d" );
$int_numbers = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

$counter = 1;
/* integer type variations */
fprintf($fp, "\n*** Testing fprintf() with integers ***\n");
foreach( $int_variation as $int_var ) {
  fprintf( $fp, "\n-- Iteration %d --\n",$counter);
  foreach( $int_numbers as $int_num ) {
    fprintf( $fp, "\n");
    fprintf( $fp, $int_var, $int_num );
  }
  $counter++;
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() with integers ***

-- Iteration 1 --

0
1
-1
2
-2
23333333
-23333333
1234
-- Iteration 2 --

0
1
-1
2
-2
23333333
-23333333
1234
-- Iteration 3 --

+0
+1
-1
+2
-2
+23333333
-23333333
+1234
-- Iteration 4 --

      0
      1
     -1
      2
     -2
23333333
-23333333
   1234
-- Iteration 5 --

0      
1      
-1     
2      
-2     
23333333
-23333333
1234   
-- Iteration 6 --

0000000
0000001
-000001
0000002
-000002
23333333
-23333333
0001234
-- Iteration 7 --

0      
1      
-1     
2      
-2     
23333333
-23333333
1234   
-- Iteration 8 --

######0
######1
#####-1
######2
#####-2
23333333
-23333333
###1234
Done
