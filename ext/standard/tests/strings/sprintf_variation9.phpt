--TEST--
Test sprintf() function : usage variations - float formats with float values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : float formats with float values ***\n";

// array of float type values

$float_values = array (
-2147483649, // float value
  2147483648,  // float value
  -0x80000001, // float value, beyond max negative int
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  -020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  10.0000000000000000005,
  10.5e+5,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  .5e+7,
  -.5e+7,
  .6e-19,
  -.6e-19,
  .05E+44,
  -.05E+44,
  .0034E-30,
  -.0034E-30
);

// various float formats
$float_formats = array(
  "%f", "%hf", "%lf",
  "%Lf", " %f", "%f ",
  "\t%f", "\n%f", "%4f",
  "%30f", "%[0-9]", "%*f",
);

$count = 1;
foreach($float_values as $float_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($float_formats as $format) {
    var_dump( sprintf($format, $float_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : float formats with float values ***

-- Iteration 1 --
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(19) " -2147483649.000000"
unicode(19) "-2147483649.000000 "
unicode(19) "	-2147483649.000000"
unicode(19) "
-2147483649.000000"
unicode(18) "-2147483649.000000"
unicode(30) "            -2147483649.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 2 --
unicode(17) "2147483648.000000"
unicode(1) "f"
unicode(17) "2147483648.000000"
unicode(1) "f"
unicode(18) " 2147483648.000000"
unicode(18) "2147483648.000000 "
unicode(18) "	2147483648.000000"
unicode(18) "
2147483648.000000"
unicode(17) "2147483648.000000"
unicode(30) "             2147483648.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 3 --
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(19) " -2147483649.000000"
unicode(19) "-2147483649.000000 "
unicode(19) "	-2147483649.000000"
unicode(19) "
-2147483649.000000"
unicode(18) "-2147483649.000000"
unicode(30) "            -2147483649.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 4 --
unicode(18) "34359738369.000000"
unicode(1) "f"
unicode(18) "34359738369.000000"
unicode(1) "f"
unicode(19) " 34359738369.000000"
unicode(19) "34359738369.000000 "
unicode(19) "	34359738369.000000"
unicode(19) "
34359738369.000000"
unicode(18) "34359738369.000000"
unicode(30) "            34359738369.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 5 --
unicode(17) "2147483649.000000"
unicode(1) "f"
unicode(17) "2147483649.000000"
unicode(1) "f"
unicode(18) " 2147483649.000000"
unicode(18) "2147483649.000000 "
unicode(18) "	2147483649.000000"
unicode(18) "
2147483649.000000"
unicode(17) "2147483649.000000"
unicode(30) "             2147483649.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 6 --
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(18) "-2147483649.000000"
unicode(1) "f"
unicode(19) " -2147483649.000000"
unicode(19) "-2147483649.000000 "
unicode(19) "	-2147483649.000000"
unicode(19) "
-2147483649.000000"
unicode(18) "-2147483649.000000"
unicode(30) "            -2147483649.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 7 --
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

-- Iteration 8 --
unicode(9) "-0.100000"
unicode(1) "f"
unicode(9) "-0.100000"
unicode(1) "f"
unicode(10) " -0.100000"
unicode(10) "-0.100000 "
unicode(10) "	-0.100000"
unicode(10) "
-0.100000"
unicode(9) "-0.100000"
unicode(30) "                     -0.100000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 9 --
unicode(9) "10.000000"
unicode(1) "f"
unicode(9) "10.000000"
unicode(1) "f"
unicode(10) " 10.000000"
unicode(10) "10.000000 "
unicode(10) "	10.000000"
unicode(10) "
10.000000"
unicode(9) "10.000000"
unicode(30) "                     10.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 10 --
unicode(14) "1050000.000000"
unicode(1) "f"
unicode(14) "1050000.000000"
unicode(1) "f"
unicode(15) " 1050000.000000"
unicode(15) "1050000.000000 "
unicode(15) "	1050000.000000"
unicode(15) "
1050000.000000"
unicode(14) "1050000.000000"
unicode(30) "                1050000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 11 --
unicode(13) "100000.000000"
unicode(1) "f"
unicode(13) "100000.000000"
unicode(1) "f"
unicode(14) " 100000.000000"
unicode(14) "100000.000000 "
unicode(14) "	100000.000000"
unicode(14) "
100000.000000"
unicode(13) "100000.000000"
unicode(30) "                 100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 12 --
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(15) " -100000.000000"
unicode(15) "-100000.000000 "
unicode(15) "	-100000.000000"
unicode(15) "
-100000.000000"
unicode(14) "-100000.000000"
unicode(30) "                -100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 13 --
unicode(8) "0.000010"
unicode(1) "f"
unicode(8) "0.000010"
unicode(1) "f"
unicode(9) " 0.000010"
unicode(9) "0.000010 "
unicode(9) "	0.000010"
unicode(9) "
0.000010"
unicode(8) "0.000010"
unicode(30) "                      0.000010"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 14 --
unicode(9) "-0.000010"
unicode(1) "f"
unicode(9) "-0.000010"
unicode(1) "f"
unicode(10) " -0.000010"
unicode(10) "-0.000010 "
unicode(10) "	-0.000010"
unicode(10) "
-0.000010"
unicode(9) "-0.000010"
unicode(30) "                     -0.000010"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 15 --
unicode(13) "100000.000000"
unicode(1) "f"
unicode(13) "100000.000000"
unicode(1) "f"
unicode(14) " 100000.000000"
unicode(14) "100000.000000 "
unicode(14) "	100000.000000"
unicode(14) "
100000.000000"
unicode(13) "100000.000000"
unicode(30) "                 100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 16 --
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(15) " -100000.000000"
unicode(15) "-100000.000000 "
unicode(15) "	-100000.000000"
unicode(15) "
-100000.000000"
unicode(14) "-100000.000000"
unicode(30) "                -100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 17 --
unicode(13) "100000.000000"
unicode(1) "f"
unicode(13) "100000.000000"
unicode(1) "f"
unicode(14) " 100000.000000"
unicode(14) "100000.000000 "
unicode(14) "	100000.000000"
unicode(14) "
100000.000000"
unicode(13) "100000.000000"
unicode(30) "                 100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 18 --
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(15) " -100000.000000"
unicode(15) "-100000.000000 "
unicode(15) "	-100000.000000"
unicode(15) "
-100000.000000"
unicode(14) "-100000.000000"
unicode(30) "                -100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 19 --
unicode(13) "100000.000000"
unicode(1) "f"
unicode(13) "100000.000000"
unicode(1) "f"
unicode(14) " 100000.000000"
unicode(14) "100000.000000 "
unicode(14) "	100000.000000"
unicode(14) "
100000.000000"
unicode(13) "100000.000000"
unicode(30) "                 100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 20 --
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(14) "-100000.000000"
unicode(1) "f"
unicode(15) " -100000.000000"
unicode(15) "-100000.000000 "
unicode(15) "	-100000.000000"
unicode(15) "
-100000.000000"
unicode(14) "-100000.000000"
unicode(30) "                -100000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 21 --
unicode(8) "0.000010"
unicode(1) "f"
unicode(8) "0.000010"
unicode(1) "f"
unicode(9) " 0.000010"
unicode(9) "0.000010 "
unicode(9) "	0.000010"
unicode(9) "
0.000010"
unicode(8) "0.000010"
unicode(30) "                      0.000010"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 22 --
unicode(9) "-0.000010"
unicode(1) "f"
unicode(9) "-0.000010"
unicode(1) "f"
unicode(10) " -0.000010"
unicode(10) "-0.000010 "
unicode(10) "	-0.000010"
unicode(10) "
-0.000010"
unicode(9) "-0.000010"
unicode(30) "                     -0.000010"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 23 --
unicode(14) "5000000.000000"
unicode(1) "f"
unicode(14) "5000000.000000"
unicode(1) "f"
unicode(15) " 5000000.000000"
unicode(15) "5000000.000000 "
unicode(15) "	5000000.000000"
unicode(15) "
5000000.000000"
unicode(14) "5000000.000000"
unicode(30) "                5000000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 24 --
unicode(15) "-5000000.000000"
unicode(1) "f"
unicode(15) "-5000000.000000"
unicode(1) "f"
unicode(16) " -5000000.000000"
unicode(16) "-5000000.000000 "
unicode(16) "	-5000000.000000"
unicode(16) "
-5000000.000000"
unicode(15) "-5000000.000000"
unicode(30) "               -5000000.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 25 --
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

-- Iteration 26 --
unicode(9) "-0.000000"
unicode(1) "f"
unicode(9) "-0.000000"
unicode(1) "f"
unicode(10) " -0.000000"
unicode(10) "-0.000000 "
unicode(10) "	-0.000000"
unicode(10) "
-0.000000"
unicode(9) "-0.000000"
unicode(30) "                     -0.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 27 --
unicode(50) "5000000000000000069686058479707049565356032.000000"
unicode(1) "f"
unicode(50) "5000000000000000069686058479707049565356032.000000"
unicode(1) "f"
unicode(51) " 5000000000000000069686058479707049565356032.000000"
unicode(51) "5000000000000000069686058479707049565356032.000000 "
unicode(51) "	5000000000000000069686058479707049565356032.000000"
unicode(51) "
5000000000000000069686058479707049565356032.000000"
unicode(50) "5000000000000000069686058479707049565356032.000000"
unicode(50) "5000000000000000069686058479707049565356032.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 28 --
unicode(51) "-5000000000000000069686058479707049565356032.000000"
unicode(1) "f"
unicode(51) "-5000000000000000069686058479707049565356032.000000"
unicode(1) "f"
unicode(52) " -5000000000000000069686058479707049565356032.000000"
unicode(52) "-5000000000000000069686058479707049565356032.000000 "
unicode(52) "	-5000000000000000069686058479707049565356032.000000"
unicode(52) "
-5000000000000000069686058479707049565356032.000000"
unicode(51) "-5000000000000000069686058479707049565356032.000000"
unicode(51) "-5000000000000000069686058479707049565356032.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 29 --
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

-- Iteration 30 --
unicode(9) "-0.000000"
unicode(1) "f"
unicode(9) "-0.000000"
unicode(1) "f"
unicode(10) " -0.000000"
unicode(10) "-0.000000 "
unicode(10) "	-0.000000"
unicode(10) "
-0.000000"
unicode(9) "-0.000000"
unicode(30) "                     -0.000000"
unicode(4) "0-9]"
unicode(1) "f"
Done
