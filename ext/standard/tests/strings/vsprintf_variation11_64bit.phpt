--TEST--
Test vsprintf() function : usage variations - octal formats with octal values
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
 * Test vsprintf() when different octal formats and octal values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vsprintf() : octal formats with octal values ***\n";

// defining array of octal formats
$formats = array(
  "%o",
  "%+o %-o %O",
  "%lo %Lo, %4o %-4o",
  "%10.4o %-10.4o %04o %04.4o",
  "%'#2o %'2o %'$2o %'_2o",
  "%o %o %o %o",
  "%% %%o %10 o%",
  '%3$o %4$o %1$o %2$o'
);

// Arrays of octal values for the format defined in $format.
// Each sub array contains octal values which correspond to each format string in $format
$args_array = array(
  array(00),
  array(-01, 01, +022),
  array(-020000000000, 020000000000, 017777777777, -017777777777),
  array(0123456, 01234567, -01234567, 01234567),
  array(0111, 02222, -0333333, -044444444),
  array(0x123b, 0xfAb, 0123, 012),
  array(01234, 0567, -01234, 02345),
  array(03, 04, 01, 02)

);

// looping to test vsprintf() with different octal formats from the above $formats array
// and with octal values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";
  var_dump( vsprintf($format, $args_array[$counter-1]) );
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : octal formats with octal values ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(25) "1777777777777777777777 1 "

-- Iteration 3 --
string(60) "1777777777760000000000 o, 17777777777 1777777777760000000001"

-- Iteration 4 --
string(49) "                      1777777777777776543211 0000"

-- Iteration 5 --
string(54) "111 2222 1777777777777777444445 1777777777777733333334"

-- Iteration 6 --
string(17) "11073 7653 123 12"

-- Iteration 7 --
string(6) "% %o o"

-- Iteration 8 --
string(7) "1 2 3 4"
Done
