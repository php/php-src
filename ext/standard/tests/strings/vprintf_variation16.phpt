--TEST--
Test vprintf() function : usage variations - unsigned formats with signed and other types of values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
 * Test vprintf() when different unsigned formats and signed values and other types of values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : unsigned formats and signed & other types of values ***\n";

// defining array of unsigned formats
$formats =
  '%u %+u %-u
   %lu %4u %-4u
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
        '123hello', '-123hello', '+123hello',
        "\12345678hello", "-\12345678hello", 'h123456ello',
        "1234hello", "hello\0world", "NULL", "true",
        "3", "4", '1', '2'),

  // different arrays
  array( array(0), array(1, 2), array(-1, -1),
         array("123"), array('-123'), array("-123"),
         array(true), array(TRUE), array(FALSE),
         array("123hello"), array("1", "2"), array('123hello'), array(12=>"12twelve"),
         array("3"), array("4"), array("1"), array("2") ),

  // array of boolean data
  array( true, TRUE, false,
         TRUE, FALSE, 1,
         true, TRUE, FALSE,
         0, 1, 1, 0,
         1, TRUE, 0, FALSE),

);

// looping to test vprintf() with different unsigned formats from the above $format array
// and with signed and other types of  values from the above $args_array array
$counter = 1;
foreach($args_array as $args) {
  echo "\n-- Iteration $counter --\n";
  $result = vprintf($formats, $args);
  echo "\n";
  var_dump($result);
  $counter++;
}
?>
--EXPECT--
*** Testing vprintf() : unsigned formats and signed & other types of values ***

-- Iteration 1 --
2 0 10
   123456 123456 1234
   2820130816 2840207360 1177509888
   12345 12 4294967284 4294843840
   10 123456 2 0
int(115)

-- Iteration 2 --
0 0 0
   123 4294967173 123 
            0 0          0
   1234 0 $0 _0
   0 123 0 0
int(84)

-- Iteration 3 --
1 1 1
   1    1 1   
            1 1          1
   #1 1 $1 _1
   1 1 1 1
int(72)

-- Iteration 4 --
1 1 0
   1    0 1   
            1 1          0
   #0 1 $1 _0
   0 1 1 1
int(72)
