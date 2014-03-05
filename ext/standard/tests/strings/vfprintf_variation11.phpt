--TEST--
Test vfprintf() function : usage variations - octal formats with octal values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different octal formats and octal values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : octal formats with octal values ***\n";

// defining array of octal formats
$formats = array(
  "%o",
  "%+o %-o %O",
  "%lo %Lo, %4o %-4o",
  "%10.4o %-10.4o %04o %04.4o",
  "%'#2o %'2o %'$2o %'_2o",
  "%o %o %o %o",
  "%% %%o %10 o%",
  '%3$o %4$o %1$o %2$o'
);

// Arrays of octal values for the format defined in $format.
// Each sub array contains octal values which correspond to each format string in $format
$args_array = array(
  array(00),
  array(-01, 01, +022),
  array(-020000000000, 020000000000, 017777777777, -017777777777),
  array(0123456, 012345678, -01234567, 01234567),
  array(0111, 02222, -0333333, -044444444),
  array(0x123b, 0xfAb, 0123, 01293),
  array(01234, 05678, -01234, 02345),
  array(03, 04, 01, 02)

);

/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_variation11.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different octal formats from the above $formats array
// and with octal values from the above $args_array array
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
*** Testing vfprintf() : octal formats with octal values ***

-- Iteration 1 --
0
-- Iteration 2 --
37777777777 1 
-- Iteration 3 --
20000000000 o, 17777777777 20000000001
-- Iteration 4 --
                      37776543211 0000
-- Iteration 5 --
111 2222 37777444445 37733333334
-- Iteration 6 --
11073 7653 123 12
-- Iteration 7 --
% %o o
-- Iteration 8 --
1 2 3 4
===DONE===
