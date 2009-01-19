--TEST--
Test vprintf() function : usage variations - octal formats with octal values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different octal formats and octal values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : octal formats with octal values ***\n";

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
  array(0123456, 012345678, -01234567, 01234567),
  array(0111, 02222, -0333333, -044444444),
  array(0x123b, 0xfAb, 0123, 01293),
  array(01234, 05678, -01234, 02345),
  array(03, 04, 01, 02)

);

// looping to test vprintf() with different octal formats from the above $formats array
// and with octal values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";   
  $result = vprintf($format, $args_array[$counter-1]);
  echo "\n";
  var_dump($result); 
  $counter++;
}

?>
===DONE===
--EXPECT--
*** Testing vprintf() : octal formats with octal values ***

-- Iteration 1 --
0
int(1)

-- Iteration 2 --
37777777777 1 
int(14)

-- Iteration 3 --
20000000000 o, 17777777777 20000000001
int(38)

-- Iteration 4 --
                      37776543211 0000
int(38)

-- Iteration 5 --
111 2222 37777444445 37733333334
int(32)

-- Iteration 6 --
11073 7653 123 12
int(17)

-- Iteration 7 --
% %o o
int(6)

-- Iteration 8 --
1 2 3 4
int(7)
===DONE===
