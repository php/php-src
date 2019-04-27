--TEST--
Test vsprintf() function : usage variations - unsigned formats with unsigned values
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
 * Test vsprintf() when different unsigned formats and unsigned values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vsprintf() : unsigned formats and unsigned values ***\n";

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
  array("1234000", 10e20, 1.2e2),
  array(1, 0, 00, "10_"),
  array(3, 4, 1, 2)
);

// looping to test vsprintf() with different unsigned formats from the above $format array
// and with signed and other types of  values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";
  var_dump( vsprintf($format, $args_array[$counter-1]) );
  $counter++;
}

echo "Done";
?>
--EXPECT--
*** Testing vsprintf() : unsigned formats and unsigned values ***

-- Iteration 1 --
string(16) "1234567 342391 0"

-- Iteration 2 --
string(24) "12345678900 u 1234 12345"

-- Iteration 3 --
string(34) "   1234000 3875820019684212736 120"

-- Iteration 4 --
string(10) "#1 0 $0 10"

-- Iteration 5 --
string(7) "1 2 3 4"
Done
