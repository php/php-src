--TEST--
Test sprintf() function : usage variations - float formats with integer values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : float formats with integer values ***\n";

// array of int type values
$integer_values = array (
  0,
  1,
  -1,
  -2147483648, // max negative integer value
  -2147483647,
  2147483647,  // max positive integer value
  2147483640,
  0x123B,      // integer as hexadecimal
  0x12ab,
  0Xfff,
  0XFA,
  -0x80000000, // max negative integer as hexadecimal
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal
  01912,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal
  017777777777  // max positive integer as octal
);

// various float formats
$float_formats = array(
  "%f", "%hf", "%lf",
  "%Lf", " %f", "%f ",
  "\t%f", "\n%f", "%4f",
  "%30f", "%[0-9]", "%*f"
);

$count = 1;
foreach($integer_values as $int_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($float_formats as $format) {
    // with two arguments
    var_dump( sprintf($format, $int_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : float formats with integer values ***

-- Iteration 1 --
unicode(8) "0.000000"
unicode(1) "f"
unicode(8) "0.000000"
unicode(1) "f"
unicode(9) " 0.000000"
unicode(9) "0.000000 "
unicode(9) "	0.000000"
unicode(9) "
0.000000"
unicode(8) "0.000000"
unicode(30) "                      0.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 2 --
unicode(8) "1.000000"
unicode(1) "f"
unicode(8) "1.000000"
unicode(1) "f"
unicode(9) " 1.000000"
unicode(9) "1.000000 "
unicode(9) "	1.000000"
unicode(9) "
1.000000"
unicode(8) "1.000000"
unicode(30) "                      1.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 3 --
unicode(9) "-1.000000"
unicode(1) "f"
unicode(9) "-1.000000"
unicode(1) "f"
unicode(10) " -1.000000"
unicode(10) "-1.000000 "
unicode(10) "	-1.000000"
unicode(10) "
-1.000000"
unicode(9) "-1.000000"
unicode(30) "                     -1.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 4 --
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(19) " -2147483648.000000"
unicode(19) "-2147483648.000000 "
unicode(19) "	-2147483648.000000"
unicode(19) "
-2147483648.000000"
unicode(18) "-2147483648.000000"
unicode(30) "            -2147483648.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 5 --
unicode(18) "-2147483647.000000"
unicode(1) "f"
unicode(18) "-2147483647.000000"
unicode(1) "f"
unicode(19) " -2147483647.000000"
unicode(19) "-2147483647.000000 "
unicode(19) "	-2147483647.000000"
unicode(19) "
-2147483647.000000"
unicode(18) "-2147483647.000000"
unicode(30) "            -2147483647.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 6 --
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(18) " 2147483647.000000"
unicode(18) "2147483647.000000 "
unicode(18) "	2147483647.000000"
unicode(18) "
2147483647.000000"
unicode(17) "2147483647.000000"
unicode(30) "             2147483647.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 7 --
unicode(17) "2147483640.000000"
unicode(1) "f"
unicode(17) "2147483640.000000"
unicode(1) "f"
unicode(18) " 2147483640.000000"
unicode(18) "2147483640.000000 "
unicode(18) "	2147483640.000000"
unicode(18) "
2147483640.000000"
unicode(17) "2147483640.000000"
unicode(30) "             2147483640.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 8 --
unicode(11) "4667.000000"
unicode(1) "f"
unicode(11) "4667.000000"
unicode(1) "f"
unicode(12) " 4667.000000"
unicode(12) "4667.000000 "
unicode(12) "	4667.000000"
unicode(12) "
4667.000000"
unicode(11) "4667.000000"
unicode(30) "                   4667.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 9 --
unicode(11) "4779.000000"
unicode(1) "f"
unicode(11) "4779.000000"
unicode(1) "f"
unicode(12) " 4779.000000"
unicode(12) "4779.000000 "
unicode(12) "	4779.000000"
unicode(12) "
4779.000000"
unicode(11) "4779.000000"
unicode(30) "                   4779.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 10 --
unicode(11) "4095.000000"
unicode(1) "f"
unicode(11) "4095.000000"
unicode(1) "f"
unicode(12) " 4095.000000"
unicode(12) "4095.000000 "
unicode(12) "	4095.000000"
unicode(12) "
4095.000000"
unicode(11) "4095.000000"
unicode(30) "                   4095.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 11 --
unicode(10) "250.000000"
unicode(1) "f"
unicode(10) "250.000000"
unicode(1) "f"
unicode(11) " 250.000000"
unicode(11) "250.000000 "
unicode(11) "	250.000000"
unicode(11) "
250.000000"
unicode(10) "250.000000"
unicode(30) "                    250.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 12 --
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(19) " -2147483648.000000"
unicode(19) "-2147483648.000000 "
unicode(19) "	-2147483648.000000"
unicode(19) "
-2147483648.000000"
unicode(18) "-2147483648.000000"
unicode(30) "            -2147483648.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 13 --
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(18) " 2147483647.000000"
unicode(18) "2147483647.000000 "
unicode(18) "	2147483647.000000"
unicode(18) "
2147483647.000000"
unicode(17) "2147483647.000000"
unicode(30) "             2147483647.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 14 --
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(18) " 2147483647.000000"
unicode(18) "2147483647.000000 "
unicode(18) "	2147483647.000000"
unicode(18) "
2147483647.000000"
unicode(17) "2147483647.000000"
unicode(30) "             2147483647.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 15 --
unicode(9) "83.000000"
unicode(1) "f"
unicode(9) "83.000000"
unicode(1) "f"
unicode(10) " 83.000000"
unicode(10) "83.000000 "
unicode(10) "	83.000000"
unicode(10) "
83.000000"
unicode(9) "83.000000"
unicode(30) "                     83.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 16 --
unicode(8) "1.000000"
unicode(1) "f"
unicode(8) "1.000000"
unicode(1) "f"
unicode(9) " 1.000000"
unicode(9) "1.000000 "
unicode(9) "	1.000000"
unicode(9) "
1.000000"
unicode(8) "1.000000"
unicode(30) "                      1.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 17 --
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(18) "-2147483648.000000"
unicode(1) "f"
unicode(19) " -2147483648.000000"
unicode(19) "-2147483648.000000 "
unicode(19) "	-2147483648.000000"
unicode(19) "
-2147483648.000000"
unicode(18) "-2147483648.000000"
unicode(30) "            -2147483648.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 18 --
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(17) "2147483647.000000"
unicode(1) "f"
unicode(18) " 2147483647.000000"
unicode(18) "2147483647.000000 "
unicode(18) "	2147483647.000000"
unicode(18) "
2147483647.000000"
unicode(17) "2147483647.000000"
unicode(30) "             2147483647.000000"
unicode(4) "0-9]"
unicode(1) "f"
Done
