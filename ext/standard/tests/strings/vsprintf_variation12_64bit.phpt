--TEST--
Test vsprintf() function : usage variations - octal formats with non-octal values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vsprintf(string format, array args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vsprintf() when different octal formats and non-octal values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vsprintf() : octal formats and non-octal values ***\n";

// defining array of octal formats
$formats = 
  '%o %+o %-o 
   %lo %Lo %4o %-4o
   %10.4o %-10.4o %.4o 
   %\'#2o %\'2o %\'$2o %\'_2o
   %3$o %4$o %1$o %2$o';

// Arrays of non octal values for the format defined in $format.
// Each sub array contains non octal values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(2.2, .2, 10.2,
        123456.234, 123456.234, -1234.6789, +1234.6789,
        2e10, +2e12, 22e+12,
        12345.780, 12.000000011111, -12.00000111111, -123456.234,
        3.33, +4.44, 1.11,-2.22 ),

  // array of int values
  array(2, -2, +2,
        123456, 123456234, -12346789, +12346789,
        123200, +20000, 22212,
        12345780, 1211111, -12111111, -12345634,
        3, +4, 1,-2 ),

  // array of strings
  array(" ", ' ', 'hello',
        '123hello', "123hello", '-123hello', '+123hello',
        "\12345678hello", "-\12345678hello", 'h123456ello',
        "1234hello", "hello\0world", "NULL", "true",
        "3", "4", '1', '2'),

  // different arrays
  array( array(0), array(1, 2), array(-1, -1),
         array("123"), array('123'), array('-123'), array("-123"),
         array(true), array(false), array(FALSE),
         array("123hello"), array("1", "2"), array('123hello'), array(12=>"12twelve"),
         array("3"), array("4"), array("1"), array("2") ),

  // array of boolean data
  array( true, TRUE, false,
         TRUE, 0, FALSE, 1,
         true, false, TRUE,
         0, 1, 1, 0,
         1, TRUE, 0, FALSE),
  
);
 
// looping to test vsprintf() with different octal formats from the above $format array
// and with non-octal values from the above $args_array array
$counter = 1;
foreach($args_array as $args) {
  echo "\n-- Iteration $counter --\n";
  var_dump( vsprintf($formats, $args) );
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : octal formats and non-octal values ***

-- Iteration 1 --
string(149) "2 0 12 
   361100 o 1777777777777777775456 2322
                          
   30071 14 1777777777777777777764 1777777777777777416700
   12 361100 2 0"

-- Iteration 2 --
string(201) "2 1777777777777777777776 2 
   361100 o 1777777777777720715133 57062645
                          
   57060664 4475347 1777777777777721631371 1777777777777720717336
   2 361100 2 1777777777777777777776"

-- Iteration 3 --
string(99) "0 0 0 
   173 o 1777777777777777777605 173 
                          
   2322 0 $0 _0
   0 173 0 0"

-- Iteration 4 --
string(75) "1 1 1 
   1 o    1 1   
                          
   #1 1 $1 _1
   1 1 1 1"

-- Iteration 5 --
string(75) "1 1 0 
   1 o    0 1   
                          
   #0 1 $1 _0
   0 1 1 1"
Done
