--TEST--
Test vprintf() function : usage variations - char formats with non-char values
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different char formats and non-char values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : char formats and non-char values ***\n";

// defining an array of various char formats
$formats =
  '%c %+c %-c
   %lc %4c %-4c
   %10.4c %-10.4c %04c %04.4c
   %\'10c %\'10c %\'$10c %\'_10c
   %3$c %4$c %1$c %2$c';

// Arrays of non char values for the format defined in $format.
// Each sub array contains non char values which correspond to each format in $format
$args_array = array(

  // array of float values
  array(65.8, -65.8, +66.8,
        93.2, 126.8, -126.49,
        35.44, -35.68, 32.99, -32.00,
        -61.51, 61.51, 50.49, -54.50,
        83.33, +84.44, 81.11, 82.22),

  // array of int values
  array(65, -65, +66,
        169, 126, -126,
        35, -35, 32, -32,
        -61, 61, 50, -54,
        83, +84, 81, 82),

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

// looping to test vprintf() with different char formats from the above $format array
// and with non-char values from the above $args_array array
$counter = 1;
foreach($args_array as $args) {
  echo "\n-- Iteration $counter --\n";
  $result = vprintf($formats, $args);
  echo "\n";
  var_dump($result);
  $counter++;
}

?>
--EXPECTF--
*** Testing vprintf() : char formats and non-char values ***

-- Iteration 1 --
A ¿ B
   ] ~ ‚
   # Ý   à
   Ã = 2 Ê
   B ] A ¿
int(47)

-- Iteration 2 --
A ¿ B
   © ~ ‚
   # Ý   à
   Ã = 2 Ê
   B © A ¿
int(47)

-- Iteration 3 --
%0 %0 %0
   { … {
   %0 %0 @ %0
   Ò %0 %0 %0
   %0 { %0 %0
int(47)

-- Iteration 4 --
  
     
      
      
      
int(47)

-- Iteration 5 --
  %0
    %0 
    %0  %0
   %0   %0
   %0   
int(47)
