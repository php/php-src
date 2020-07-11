--TEST--
Test vprintf() function : usage variations - scientific formats with non-scientific values
--FILE--
<?php
/*
 * Test vprintf() when different scientific formats and non-scientific values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : scientific formats and non-scientific values ***\n";

// defining array of non-scientific formats
$formats =
    '%e %+e %-e
    %le %4e %-4e
    %10.4e %-10.4e %04e %04.4e
    %\'#2e %\'2e %\'$2e %\'_2e
    %3$e %4$e %1$e %2$e';

// Arrays of non scientific values for the format defined in $format.
// Each sub array contains non scientific values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(2.2, .2, 10.2,
        123456.234, -1234.6789, +1234.6789,
        20.00, +212.2, -411000000000, 2212.000000000001,
        12345.780, 12.000000011111, -12.00000111111, -123456.234,
        3.33, +4.44, 1.11,-2.22 ),

  // array of strings
  array(" ", ' ', 'hello',
        '123hello', '-123hello', '+123hello',
        "\12345678hello", "-\12345678hello", '0123456hello', 'h123456ello',
        "1234hello", "hello\0world", "NULL", "true",
        "3", "4", '1', '2'),

  // different arrays
  array( array(0), array(1, 2), array(-1, -1),
         array("123"), array('-123'), array("-123"),
         array(true), array(false), array(TRUE), array(FALSE),
         array("123hello"), array("1", "2"), array('123hello'), array(12=>"12twelve"),
         array("3"), array("4"), array("1"), array("2") ),

  // array of boolean data
  array( true, TRUE, false,
         TRUE, FALSE, 1,
         true, false, TRUE, FALSE,
         0, 1, 1, 0,
         1, TRUE, 0, FALSE),

);

// looping to test vprintf() with different scientific formats from the above $format array
// and with non-scientific values from the above $args_array array
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
*** Testing vprintf() : scientific formats and non-scientific values ***

-- Iteration 1 --
2.200000e+0 +2.000000e-1 1.020000e+1
    1.234562e+5 -1.234679e+3 1.234679e+3
     2.0000e+1 2.1220e+2  -4.110000e+11 2.2120e+3
    1.234578e+4 1.200000e+1 -1.200000e+1 -1.234562e+5
    1.020000e+1 1.234562e+5 2.200000e+0 2.000000e-1
int(233)

-- Iteration 2 --
0.000000e+0 +0.000000e+0 0.000000e+0
    1.230000e+2 -1.230000e+2 1.230000e+2
     0.0000e+0 0.0000e+0  1.234560e+5 0.0000e+0
    1.234000e+3 0.000000e+0 0.000000e+0 0.000000e+0
    0.000000e+0 1.230000e+2 0.000000e+0 0.000000e+0
int(229)

-- Iteration 3 --
1.000000e+0 +1.000000e+0 1.000000e+0
    1.000000e+0 1.000000e+0 1.000000e+0
     1.0000e+0 1.0000e+0  1.000000e+0 1.0000e+0
    1.000000e+0 1.000000e+0 1.000000e+0 1.000000e+0
    1.000000e+0 1.000000e+0 1.000000e+0 1.000000e+0
int(228)

-- Iteration 4 --
1.000000e+0 +1.000000e+0 0.000000e+0
    1.000000e+0 0.000000e+0 1.000000e+0
     1.0000e+0 0.0000e+0  1.000000e+0 0.0000e+0
    0.000000e+0 1.000000e+0 1.000000e+0 0.000000e+0
    0.000000e+0 1.000000e+0 1.000000e+0 1.000000e+0
int(228)
