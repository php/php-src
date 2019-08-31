--TEST--
Test vfprintf() function : usage variations - unsigned formats with unsigned values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different unsigned formats and unsigned values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : unsigned formats and unsigned values ***\n";

// defining array of unsigned formats
$formats = array(
  '%u %+u %-u',
  '%lu %Lu %4u %-4u',
  '%10.4u %-10.4u %.4u',
  '%\'#2u %\'2u %\'$2u %\'_2u',
  '%3$u %4$u %1$u %2$u'
);

// Arrays of unsigned values for the format defined in $format.
// Each sub array contains unsigned values which correspond to each format string in $format
$args_array = array(
  array(1234567, 01234567, 0 ),
  array(12345678900, 12345678900, 1234, 12345),
  array("1234000", 10.1234567e10, 1.2e2),
  array(1, 0, 00, "10_"),
  array(3, 4, 1, 2)
);

/* creating dumping file */
$data_file = __DIR__ . '/vfprintf_variation15_64bit.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different unsigned formats from the above $format array
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
*** Testing vfprintf() : unsigned formats and unsigned values ***

-- Iteration 1 --
1234567 342391 0
-- Iteration 2 --
12345678900 u 1234 12345
-- Iteration 3 --
   1234000 101234567000 120
-- Iteration 4 --
#1 0 $0 10
-- Iteration 5 --
1 2 3 4
===DONE===
