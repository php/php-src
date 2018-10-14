--TEST--
Test vfprintf() function : usage variations - string formats with non-string values
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different string formats and non-string values are passed to
 * the '$format' and '$args' arguments of the function
*/

error_reporting(E_ALL & ~E_NOTICE);

echo "*** Testing vfprintf() : string formats and non-string values ***\n";

// defining array of string formats
$formats =
  '%s %+s %-s 
   %ls %Ls %4s %-4s
   %10.4s %-10.4s %04s %04.4s
   %\'#2s %\'2s %\'$2s %\'_2s
   %3$s %4$s %1$s %2$s';

// Arrays of non string values for the format defined in $format.
// Each sub array contains non string values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(2.2, .2, 10.2,
        123456.234, 123456.234, -1234.6789, +1234.6789,
        2.1234567e10, +2.7654321e10, -2.7654321e10,
        12345.780, 12.000000011111, -12.00000111111, -123456.234,
        3.33, +4.44, 1.11,-2.22 ),

 // array of int values
 array(2, -2, +2,
       123456, 123456234, -12346789, +12346789,
       123200, +20000, -40000, 22212,
       12345780, 1211111, -12111111, -12345634,
       3, +4, 1,-2 ),


  // different arrays
  array( array(0), array(1, 2), array(-1, -1),
         array("123"), array('123'), array('-123'), array("-123"),
         array(true), array(false), array(TRUE), array(FALSE),
         array("123hello"), array("1", "2"), array('123hello'), array(12=>"12twelve"),
         array("3"), array("4"), array("1"), array("2") ),

  // array of boolean data
  array( true, TRUE, false,
         TRUE, 0, FALSE, 1,
         true, false, TRUE, FALSE,
         0, 1, 1, 0,
         1, TRUE, 0, FALSE),

);

/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_variation8.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different string formats from the above $format array
// and with non-string values from the above $args_array array
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
*** Testing vfprintf() : string formats and non-string values ***

-- Iteration 1 --
2.2 0.2 10.2 
   123456.234 s -1234.6789 1234.6789
         2123 2765       -27654321000 1234
   12.000000011111 -12.00000111111 -123456.234 3.33
   10.2 123456.234 2.2 0.2
-- Iteration 2 --
2 -2 2 
   123456 s -12346789 12346789
         1232 2000       -40000 2221
   12345780 1211111 -12111111 -12345634
   2 123456 2 -2
-- Iteration 3 --
Array Array Array 
   Array s Array Array
         Arra Arra       Array Arra
   Array Array Array Array
   Array Array Array Array
-- Iteration 4 --
1 1  
   1 s      1   
            1            0001 0000
   #0 1 $1 _0
    1 1 1
===DONE===
