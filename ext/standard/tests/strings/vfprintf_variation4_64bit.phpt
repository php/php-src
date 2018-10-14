--TEST--
Test vfprintf() function : usage variations - int formats with non-integer values
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
 * Test vfprintf() when different int formats and non-int values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : int formats and non-integer values ***\n";

// defining array of int formats
$formats =
  '%d %+d %-d 
   %ld %Ld %4d %-4d
   %10.4d %-10.4d %.4d %04.4d
   %\'#2d %\'2d %\'$2d %\'_2d
   %3$d %4$d %1$d %2$d';

// Arrays of non int values for the format defined in $format.
// Each sub array contains non int values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(2.2, .2, 10.2,
        123456.234, 123456.234, -1234.6789, +1234.6789,
        2e10, +2e5, 4e3, 22e+6,
        12345.780, 12.000000011111, -12.00000111111, -123456.234,
        3.33, +4.44, 1.11,-2.22 ),

  // array of strings
  array(" ", ' ', 'hello',
        '123hello', "123hello", '-123hello', '+123hello',
        "\12345678hello", "-\12345678hello", '0123456hello', 'h123456ello',
        "1234hello", "hello\0world", "NULL", "true",
        "3", "4", '1', '2'),

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
$data_file = dirname(__FILE__) . '/vfprintf_variation4_64bit.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different int formats from the above $format array
// and with non-int values from the above $args_array array
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
*** Testing vfprintf() : int formats and non-integer values ***

-- Iteration 1 --
2 +0 10 
   123456 d -1234 1234
   20000000000 200000     4000 22000000
   12345 12 -12 -123456
   10 123456 2 0
-- Iteration 2 --
0 +0 0 
   123 d -123 123 
            0 0          123456 0000
   1234 0 $0 _0
   0 123 0 0
-- Iteration 3 --
1 +1 1 
   1 d    1 1   
            1 1          1 0001
   #1 1 $1 _1
   1 1 1 1
-- Iteration 4 --
1 +1 0 
   1 d    0 1   
            1 0          1 0000
   #0 1 $1 _0
   0 1 1 1
===DONE===
