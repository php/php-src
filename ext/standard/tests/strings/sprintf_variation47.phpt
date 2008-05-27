--TEST--
Test sprintf() function : usage variations - scientific formats with float values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : scientific formats with float values ***\n";

// array of float values 
$float_values = array(
  -2147483649,
  2147483648,
  -0x80000001, // float value, beyond max negative int
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  -020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e5,
  -1e5,
  +1e5,
  1e+5,
  -1e-5,
  1E8,
  -1E9,
  10.0000000000000000005,
  10.5e+5
);

// array of scientific formats
$scientific_formats = array(
  "%e", "%he", "%le",
  "%Le", " %e", "%e ",
  "\t%e", "\n%e", "%4e",
  "%30e", "%[0-1]", "%*e"
);


$count = 1;
foreach($float_values as $float_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($scientific_formats as $format) {
    var_dump( sprintf($format, $float_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : scientific formats with float values ***

-- Iteration 1 --
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(13) " -2.147484e+9"
unicode(13) "-2.147484e+9 "
unicode(13) "	-2.147484e+9"
unicode(13) "
-2.147484e+9"
unicode(12) "-2.147484e+9"
unicode(30) "                  -2.147484e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 2 --
unicode(11) "2.147484e+9"
unicode(1) "e"
unicode(11) "2.147484e+9"
unicode(1) "e"
unicode(12) " 2.147484e+9"
unicode(12) "2.147484e+9 "
unicode(12) "	2.147484e+9"
unicode(12) "
2.147484e+9"
unicode(11) "2.147484e+9"
unicode(30) "                   2.147484e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 3 --
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(13) " -2.147484e+9"
unicode(13) "-2.147484e+9 "
unicode(13) "	-2.147484e+9"
unicode(13) "
-2.147484e+9"
unicode(12) "-2.147484e+9"
unicode(30) "                  -2.147484e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 4 --
unicode(12) "3.435974e+10"
unicode(1) "e"
unicode(12) "3.435974e+10"
unicode(1) "e"
unicode(13) " 3.435974e+10"
unicode(13) "3.435974e+10 "
unicode(13) "	3.435974e+10"
unicode(13) "
3.435974e+10"
unicode(12) "3.435974e+10"
unicode(30) "                  3.435974e+10"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 5 --
unicode(11) "2.147484e+9"
unicode(1) "e"
unicode(11) "2.147484e+9"
unicode(1) "e"
unicode(12) " 2.147484e+9"
unicode(12) "2.147484e+9 "
unicode(12) "	2.147484e+9"
unicode(12) "
2.147484e+9"
unicode(11) "2.147484e+9"
unicode(30) "                   2.147484e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 6 --
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(12) "-2.147484e+9"
unicode(1) "e"
unicode(13) " -2.147484e+9"
unicode(13) "-2.147484e+9 "
unicode(13) "	-2.147484e+9"
unicode(13) "
-2.147484e+9"
unicode(12) "-2.147484e+9"
unicode(30) "                  -2.147484e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 7 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 8 --
unicode(12) "-1.000000e-1"
unicode(1) "e"
unicode(12) "-1.000000e-1"
unicode(1) "e"
unicode(13) " -1.000000e-1"
unicode(13) "-1.000000e-1 "
unicode(13) "	-1.000000e-1"
unicode(13) "
-1.000000e-1"
unicode(12) "-1.000000e-1"
unicode(30) "                  -1.000000e-1"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 9 --
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(12) " 1.000000e+0"
unicode(12) "1.000000e+0 "
unicode(12) "	1.000000e+0"
unicode(12) "
1.000000e+0"
unicode(11) "1.000000e+0"
unicode(30) "                   1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 10 --
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(12) " 1.000000e+5"
unicode(12) "1.000000e+5 "
unicode(12) "	1.000000e+5"
unicode(12) "
1.000000e+5"
unicode(11) "1.000000e+5"
unicode(30) "                   1.000000e+5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 11 --
unicode(12) "-1.000000e+5"
unicode(1) "e"
unicode(12) "-1.000000e+5"
unicode(1) "e"
unicode(13) " -1.000000e+5"
unicode(13) "-1.000000e+5 "
unicode(13) "	-1.000000e+5"
unicode(13) "
-1.000000e+5"
unicode(12) "-1.000000e+5"
unicode(30) "                  -1.000000e+5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 12 --
unicode(12) "-1.000000e+5"
unicode(1) "e"
unicode(12) "-1.000000e+5"
unicode(1) "e"
unicode(13) " -1.000000e+5"
unicode(13) "-1.000000e+5 "
unicode(13) "	-1.000000e+5"
unicode(13) "
-1.000000e+5"
unicode(12) "-1.000000e+5"
unicode(30) "                  -1.000000e+5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 13 --
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(12) " 1.000000e+5"
unicode(12) "1.000000e+5 "
unicode(12) "	1.000000e+5"
unicode(12) "
1.000000e+5"
unicode(11) "1.000000e+5"
unicode(30) "                   1.000000e+5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 14 --
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(11) "1.000000e+5"
unicode(1) "e"
unicode(12) " 1.000000e+5"
unicode(12) "1.000000e+5 "
unicode(12) "	1.000000e+5"
unicode(12) "
1.000000e+5"
unicode(11) "1.000000e+5"
unicode(30) "                   1.000000e+5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 15 --
unicode(12) "-1.000000e-5"
unicode(1) "e"
unicode(12) "-1.000000e-5"
unicode(1) "e"
unicode(13) " -1.000000e-5"
unicode(13) "-1.000000e-5 "
unicode(13) "	-1.000000e-5"
unicode(13) "
-1.000000e-5"
unicode(12) "-1.000000e-5"
unicode(30) "                  -1.000000e-5"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 16 --
unicode(11) "1.000000e+8"
unicode(1) "e"
unicode(11) "1.000000e+8"
unicode(1) "e"
unicode(12) " 1.000000e+8"
unicode(12) "1.000000e+8 "
unicode(12) "	1.000000e+8"
unicode(12) "
1.000000e+8"
unicode(11) "1.000000e+8"
unicode(30) "                   1.000000e+8"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 17 --
unicode(12) "-1.000000e+9"
unicode(1) "e"
unicode(12) "-1.000000e+9"
unicode(1) "e"
unicode(13) " -1.000000e+9"
unicode(13) "-1.000000e+9 "
unicode(13) "	-1.000000e+9"
unicode(13) "
-1.000000e+9"
unicode(12) "-1.000000e+9"
unicode(30) "                  -1.000000e+9"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 18 --
unicode(11) "1.000000e+1"
unicode(1) "e"
unicode(11) "1.000000e+1"
unicode(1) "e"
unicode(12) " 1.000000e+1"
unicode(12) "1.000000e+1 "
unicode(12) "	1.000000e+1"
unicode(12) "
1.000000e+1"
unicode(11) "1.000000e+1"
unicode(30) "                   1.000000e+1"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 19 --
unicode(11) "1.050000e+6"
unicode(1) "e"
unicode(11) "1.050000e+6"
unicode(1) "e"
unicode(12) " 1.050000e+6"
unicode(12) "1.050000e+6 "
unicode(12) "	1.050000e+6"
unicode(12) "
1.050000e+6"
unicode(11) "1.050000e+6"
unicode(30) "                   1.050000e+6"
unicode(4) "0-1]"
unicode(1) "e"
Done
