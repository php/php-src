--TEST--
Test vprintf() function : usage variations - hexa formats with non-hexa values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/*
 * Test vprintf() when different hexa formats and non-hexa values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : hexa formats and non-hexa values ***\n";

// defining array of different hexa formats
$formats =
    '%x %+x %-x
    %lx %4x %-4x
    %10.4x %-10.4x %.4x
    %\'#2x %\'2x %\'$2x %\'_2x
    %3$x %4$x %1$x %2$x';

// Arrays of non hexa values for the format defined in $format.
// Each sub array contains non hexa values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(2.2, .2, 10.2,
        123456.234, -1234.6789, +1234.6789,
        2e10, +2e12, 22e+12,
        12345.780, 12.000000011111, -12.00000111111, -123456.234,
        3.33, +4.44, 1.11,-2.22 ),

  // array of int values
  array(2, -2, +2,
        123456, -12346789, +12346789,
        123200, +20000, 22212,
        12345780, 1211111, -12111111, -12345634,
        3, +4, 1,-2 ),

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

// looping to test vprintf() with different hexa formats from the above $format array
// and with non-hexa values from the above $args_array array

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
*** Testing vprintf() : hexa formats and non-hexa values ***

-- Iteration 1 --
2 0 a
    1e240 fffffffffffffb2e 4d2 
                          
    3039 c fffffffffffffff4 fffffffffffe1dc0
    a 1e240 2 0
int(125)

-- Iteration 2 --
2 fffffffffffffffe 2
    1e240 ffffffffff439a5b bc65a5
                          
    bc61b4 127ae7 ffffffffff4732f9 ffffffffff439ede
    2 1e240 2 fffffffffffffffe
int(164)

-- Iteration 3 --
0 0 0
    7b ffffffffffffff85 7b  
                          
    4d2 0 $0 _0
    0 7b 0 0
int(90)

-- Iteration 4 --
1 1 1
    1    1 1   
                          
    #1 1 $1 _1
    1 1 1 1
int(75)

-- Iteration 5 --
1 1 0
    1    0 1   
                          
    #0 1 $1 _0
    0 1 1 1
int(75)
