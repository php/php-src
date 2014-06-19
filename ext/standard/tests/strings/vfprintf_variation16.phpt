--TEST--
Test vfprintf() function : usage variations - unsigned formats with signed and other types of values
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
 * Test vfprintf() when different unsigned formats and signed values and other types of values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : unsigned formats and signed & other types of values ***\n";

// defining array of unsigned formats
$formats = 
  '%u %+u %-u 
   %lu %Lu %4u %-4u
   %10.4u %-10.4u %.4u 
   %\'#2u %\'2u %\'$2u %\'_2u
   %3$u %4$u %1$u %2$u';

// Arrays of signed and other type of values for the format defined in $format.
// Each sub array contains signed values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(+2.2, +.2, +10.2,
        +123456.234, +123456.234, +1234.6789,
        +2e10, +2e12, +22e+12,
        +12345.780, +12.000000011111, -12.00000111111, -123456.234,
        +3.33, +4.44, +1.11,-2.22 ),

  // array of strings
  array(" ", ' ', 'hello',
        '123hello', "123hello", '-123hello', '+123hello',
        "\12345678hello", "-\12345678hello", 'h123456ello',
        "1234hello", "hello\0world", "NULL", "true",
        "3", "4", '1', '2'),

  // different arrays
  array( array(0), array(1, 2), array(-1, -1),
         array("123"), array('123'), array('-123'), array("-123"),
         array(true), array(TRUE), array(FALSE),
         array("123hello"), array("1", "2"), array('123hello'), array(12=>"12twelve"),
         array("3"), array("4"), array("1"), array("2") ),

  // array of boolean data
  array( true, TRUE, false,
         TRUE, 0, FALSE, 1,
         true, TRUE, FALSE,
         0, 1, 1, 0,
         1, TRUE, 0, FALSE),
  
);
 
/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_variation16.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different unsigned formats from the above $format array
// and with signed and other types of  values from the above $args_array array
$counter = 1;
foreach($args_array as $args) {
  fprintf($fp, "\n-- Iteration %d --\n",$counter);
  vfprintf($fp, $formats, $args);
  $counter++;
}

fclose($fp);
print_r(file_get_contents($data_file));
echo "\n";

unlink($data_file);  

?>
===DONE===
--EXPECT--
*** Testing vfprintf() : unsigned formats and signed & other types of values ***

-- Iteration 1 --
2 0 10 
   123456 u 1234 2820130816
   2840207360 1177509888 12345 
   12 4294967284 4294843840 _3
   10 123456 2 0
-- Iteration 2 --
0 0 0 
   123 u 4294967173 123 
            0 0          0 
   1234 0 $0 _0
   0 123 0 0
-- Iteration 3 --
1 1 1 
   1 u    1 1   
            1 1          1 
   #1 1 $1 _1
   1 1 1 1
-- Iteration 4 --
1 1 0 
   1 u    0 1   
            1 1          0 
   #0 1 $1 _0
   0 1 1 1
===DONE===