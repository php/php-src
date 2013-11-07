--TEST--
Test vfprintf() function : usage variations - scientific formats with scientific values
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different scientific formats and scientific values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : scientific formats and scientific values ***\n";

// defining array of scientific formats
$formats = array(
  '%e %+e %-e', 
  '%le %Le %4e %-4e',
  '%10.4e %-10.4e %.4e', 
  '%\'#20e %\'20e %\'$20e %\'_20e',
  '%3$e %4$e %1$e %2$e'
);

// Arrays of scientific values for the format defined in $format.
// Each sub array contains scientific values which correspond to each format string in $format
$args_array = array(
  array(0, 1e0, "10e2" ),
  array(2.2e2, 10e10, 1000e-2, 1000e7),
  array(-22e12, 10e20, 1.2e2),
  array(1e1, +1e2, -1e3, "1e2_"),
  array(3e3, 4e3, 1e3, 2e3)
);

/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_variation17.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;
   
// looping to test vfprintf() with different scientific formats from the above $format array
// and with signed and other types of  values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  fprintf($fp, "\n-- Iteration %d --\n",$counter);
  vfprintf($fp, $format, $args_array[$counter-1]);
  $counter++;
}

fclose($fp);
print_r(file_get_contents($data_file));
echo "\n";

unlink($data_file); 
?>
===DONE===
--EXPECT--
*** Testing vfprintf() : scientific formats and scientific values ***

-- Iteration 1 --
0.000000e+0 +1.000000e+0 1.000000e+3
-- Iteration 2 --
2.200000e+2 e 1.000000e+1 1.000000e+10
-- Iteration 3 --
-2.2000e+13 1.0000e+21 1.2000e+2
-- Iteration 4 --
#########1.000000e+1 1.000000e+2 $$$$$$$$-1.000000e+3 _________1.000000e+2
-- Iteration 5 --
1.000000e+3 2.000000e+3 3.000000e+3 4.000000e+3
===DONE===
