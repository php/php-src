--TEST--
Test vfprintf() function : usage variations - float formats with non-float values
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different float formats and non-float values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vfprintf() : float formats and non-float values ***\n";

// defining array of float formats
$formats = 
  '%f %+f %-f 
   %lf %Lf %4f %-4f
   %10.4f %-10.4f %04f %04.4f
   %\'#2f %\'2f %\'$2f %\'_2f
   %3$f %4$f %1$f %2$f';

// Arrays of non float values for the format defined in $format.
// Each sub array contains non float values which correspond to each format in $format
$args_array = array(

  // array of int values
  array(2, -2, +2,
        123456, 123456234, -12346789, +12346789,
        123200, +20000, -40000, 22212,
        12345780, 1211111, -12111111, -12345634,
        3, +4, 1,-2 ),

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
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

// looping to test vfprintf() with different float formats from the above $format array
// and with non-float values from the above $args_array array
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
*** Testing vfprintf() : float formats and non-float values ***

-- Iteration 1 --
2.000000 -2.000000 2.000000 
   123456.000000 f -12346789.000000 12346789.000000
   123200.0000 20000.0000 -40000.000000 22212.0000
   12345780.000000 1211111.000000 -12111111.000000 -12345634.000000
   2.000000 123456.000000 2.000000 -2.000000
-- Iteration 2 --
0.000000 +0.000000 0.000000 
   123.000000 f -123.000000 123.000000
       0.0000 0.0000     123456.000000 0.0000
   1234.000000 0.000000 0.000000 0.000000
   0.000000 123.000000 0.000000 0.000000
-- Iteration 3 --
1.000000 +1.000000 1.000000 
   1.000000 f 1.000000 1.000000
       1.0000 1.0000     1.000000 1.0000
   1.000000 1.000000 1.000000 1.000000
   1.000000 1.000000 1.000000 1.000000
-- Iteration 4 --
1.000000 +1.000000 0.000000 
   1.000000 f 0.000000 1.000000
       1.0000 0.0000     1.000000 0.0000
   0.000000 1.000000 1.000000 0.000000
   0.000000 1.000000 1.000000 1.000000
===DONE===
