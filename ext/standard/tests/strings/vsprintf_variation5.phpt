--TEST--
Test vsprintf() function : usage variations - float formats with float values
--FILE--
<?php
/* Prototype  : string vsprintf(string format, array args)
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vsprintf() when different float formats and float values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vsprintf() : int formats with float values ***\n";


// defining array of float formats
$formats = array(
  "%f",
  "%+f %-f %F",
  "%lf %Lf, %4f %-4f",
  "%10.4f %-10.4F %04f %04.4f",
  "%'#2f %'2f %'$2f %'_2f",
  "%f %f %f %f",
  "% %%f f%",
  '%3$f %4$f %1$f %2$f'
);

// Arrays of float values for the format defined in $format.
// Each sub array contains float values which correspond to each format string in $format
$args_array = array(
  array(0.0),
  array(-0.1, +0.1, +10.0000006),
  array(2147483649, -2147483647, +2147483640, -2147483640),
  array(2e5, 2e-5, -2e5, -2e-5),
  array(0.2E5, -0.2e40, 0.2E-20, 0.2E+20),
  array(0x123b, 0xfAb, 0123, 012),
  array(1234.1234, -5678.5678, 2345.2345),
  array(3.33, 4.44, 1.11, 2.22)

);

// looping to test vsprintf() with different float formats from the above $format array
// and with float values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";
  var_dump( vsprintf($format, $args_array[$counter-1]) );
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : int formats with float values ***

-- Iteration 1 --
string(8) "0.000000"

-- Iteration 2 --
string(28) "-0.100000 0.100000 10.000001"

-- Iteration 3 --
string(57) "2147483649.000000 f, 2147483640.000000 -2147483640.000000"

-- Iteration 4 --
string(45) "200000.0000 0.0000     -200000.000000 -0.0000"

-- Iteration 5 --
string(98) "20000.000000 -1999999999999999879418332743206357172224.000000 0.000000 20000000000000000000.000000"

-- Iteration 6 --
string(43) "4667.000000 4011.000000 83.000000 10.000000"

-- Iteration 7 --
string(15) "%-5678.567800 f"

-- Iteration 8 --
string(35) "1.110000 2.220000 3.330000 4.440000"
Done
