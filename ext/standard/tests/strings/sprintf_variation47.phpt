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
string(12) "-2.147484e+9"
string(1) "e"
string(12) "-2.147484e+9"
string(1) "e"
string(13) " -2.147484e+9"
string(13) "-2.147484e+9 "
string(13) "	-2.147484e+9"
string(13) "
-2.147484e+9"
string(12) "-2.147484e+9"
string(30) "                  -2.147484e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 2 --
string(11) "2.147484e+9"
string(1) "e"
string(11) "2.147484e+9"
string(1) "e"
string(12) " 2.147484e+9"
string(12) "2.147484e+9 "
string(12) "	2.147484e+9"
string(12) "
2.147484e+9"
string(11) "2.147484e+9"
string(30) "                   2.147484e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 3 --
string(12) "-2.147484e+9"
string(1) "e"
string(12) "-2.147484e+9"
string(1) "e"
string(13) " -2.147484e+9"
string(13) "-2.147484e+9 "
string(13) "	-2.147484e+9"
string(13) "
-2.147484e+9"
string(12) "-2.147484e+9"
string(30) "                  -2.147484e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 4 --
string(12) "3.435974e+10"
string(1) "e"
string(12) "3.435974e+10"
string(1) "e"
string(13) " 3.435974e+10"
string(13) "3.435974e+10 "
string(13) "	3.435974e+10"
string(13) "
3.435974e+10"
string(12) "3.435974e+10"
string(30) "                  3.435974e+10"
string(4) "0-1]"
string(1) "e"

-- Iteration 5 --
string(11) "2.147484e+9"
string(1) "e"
string(11) "2.147484e+9"
string(1) "e"
string(12) " 2.147484e+9"
string(12) "2.147484e+9 "
string(12) "	2.147484e+9"
string(12) "
2.147484e+9"
string(11) "2.147484e+9"
string(30) "                   2.147484e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 6 --
string(12) "-2.147484e+9"
string(1) "e"
string(12) "-2.147484e+9"
string(1) "e"
string(13) " -2.147484e+9"
string(13) "-2.147484e+9 "
string(13) "	-2.147484e+9"
string(13) "
-2.147484e+9"
string(12) "-2.147484e+9"
string(30) "                  -2.147484e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 7 --
string(11) "0.000000e+0"
string(1) "e"
string(11) "0.000000e+0"
string(1) "e"
string(12) " 0.000000e+0"
string(12) "0.000000e+0 "
string(12) "	0.000000e+0"
string(12) "
0.000000e+0"
string(11) "0.000000e+0"
string(30) "                   0.000000e+0"
string(4) "0-1]"
string(1) "e"

-- Iteration 8 --
string(12) "-1.000000e-1"
string(1) "e"
string(12) "-1.000000e-1"
string(1) "e"
string(13) " -1.000000e-1"
string(13) "-1.000000e-1 "
string(13) "	-1.000000e-1"
string(13) "
-1.000000e-1"
string(12) "-1.000000e-1"
string(30) "                  -1.000000e-1"
string(4) "0-1]"
string(1) "e"

-- Iteration 9 --
string(11) "1.000000e+0"
string(1) "e"
string(11) "1.000000e+0"
string(1) "e"
string(12) " 1.000000e+0"
string(12) "1.000000e+0 "
string(12) "	1.000000e+0"
string(12) "
1.000000e+0"
string(11) "1.000000e+0"
string(30) "                   1.000000e+0"
string(4) "0-1]"
string(1) "e"

-- Iteration 10 --
string(11) "1.000000e+5"
string(1) "e"
string(11) "1.000000e+5"
string(1) "e"
string(12) " 1.000000e+5"
string(12) "1.000000e+5 "
string(12) "	1.000000e+5"
string(12) "
1.000000e+5"
string(11) "1.000000e+5"
string(30) "                   1.000000e+5"
string(4) "0-1]"
string(1) "e"

-- Iteration 11 --
string(12) "-1.000000e+5"
string(1) "e"
string(12) "-1.000000e+5"
string(1) "e"
string(13) " -1.000000e+5"
string(13) "-1.000000e+5 "
string(13) "	-1.000000e+5"
string(13) "
-1.000000e+5"
string(12) "-1.000000e+5"
string(30) "                  -1.000000e+5"
string(4) "0-1]"
string(1) "e"

-- Iteration 12 --
string(12) "-1.000000e+5"
string(1) "e"
string(12) "-1.000000e+5"
string(1) "e"
string(13) " -1.000000e+5"
string(13) "-1.000000e+5 "
string(13) "	-1.000000e+5"
string(13) "
-1.000000e+5"
string(12) "-1.000000e+5"
string(30) "                  -1.000000e+5"
string(4) "0-1]"
string(1) "e"

-- Iteration 13 --
string(11) "1.000000e+5"
string(1) "e"
string(11) "1.000000e+5"
string(1) "e"
string(12) " 1.000000e+5"
string(12) "1.000000e+5 "
string(12) "	1.000000e+5"
string(12) "
1.000000e+5"
string(11) "1.000000e+5"
string(30) "                   1.000000e+5"
string(4) "0-1]"
string(1) "e"

-- Iteration 14 --
string(11) "1.000000e+5"
string(1) "e"
string(11) "1.000000e+5"
string(1) "e"
string(12) " 1.000000e+5"
string(12) "1.000000e+5 "
string(12) "	1.000000e+5"
string(12) "
1.000000e+5"
string(11) "1.000000e+5"
string(30) "                   1.000000e+5"
string(4) "0-1]"
string(1) "e"

-- Iteration 15 --
string(12) "-1.000000e-5"
string(1) "e"
string(12) "-1.000000e-5"
string(1) "e"
string(13) " -1.000000e-5"
string(13) "-1.000000e-5 "
string(13) "	-1.000000e-5"
string(13) "
-1.000000e-5"
string(12) "-1.000000e-5"
string(30) "                  -1.000000e-5"
string(4) "0-1]"
string(1) "e"

-- Iteration 16 --
string(11) "1.000000e+8"
string(1) "e"
string(11) "1.000000e+8"
string(1) "e"
string(12) " 1.000000e+8"
string(12) "1.000000e+8 "
string(12) "	1.000000e+8"
string(12) "
1.000000e+8"
string(11) "1.000000e+8"
string(30) "                   1.000000e+8"
string(4) "0-1]"
string(1) "e"

-- Iteration 17 --
string(12) "-1.000000e+9"
string(1) "e"
string(12) "-1.000000e+9"
string(1) "e"
string(13) " -1.000000e+9"
string(13) "-1.000000e+9 "
string(13) "	-1.000000e+9"
string(13) "
-1.000000e+9"
string(12) "-1.000000e+9"
string(30) "                  -1.000000e+9"
string(4) "0-1]"
string(1) "e"

-- Iteration 18 --
string(11) "1.000000e+1"
string(1) "e"
string(11) "1.000000e+1"
string(1) "e"
string(12) " 1.000000e+1"
string(12) "1.000000e+1 "
string(12) "	1.000000e+1"
string(12) "
1.000000e+1"
string(11) "1.000000e+1"
string(30) "                   1.000000e+1"
string(4) "0-1]"
string(1) "e"

-- Iteration 19 --
string(11) "1.050000e+6"
string(1) "e"
string(11) "1.050000e+6"
string(1) "e"
string(12) " 1.050000e+6"
string(12) "1.050000e+6 "
string(12) "	1.050000e+6"
string(12) "
1.050000e+6"
string(11) "1.050000e+6"
string(30) "                   1.050000e+6"
string(4) "0-1]"
string(1) "e"
Done
