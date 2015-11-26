--TEST--
Test sprintf() function : usage variations - scientific formats with integer values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : scientific formats with integer values ***\n";

// array of integer values 
$integer_values = array(
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
  0x7fffffff,  // max positive integer as hexadecimal
  0x7FFFFFFF,  // max positive integer as hexadecimal
  0123,        // integer as octal 
  01,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal 
  017777777777  // max positive integer as octal
);

// array of scientific formats
$scientific_formats = array(
  "%e", "%he", "%le",
  "%Le", " %e", "%e ",
  "\t%e", "\n%e", "%4e",
  "%30e", "%[0-1]", "%*e"
);

$count = 1;
foreach($integer_values as $integer_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($scientific_formats as $format) {
    var_dump( sprintf($format, $integer_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : scientific formats with integer values ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 3 --
string(12) "-1.000000e+0"
string(1) "e"
string(12) "-1.000000e+0"
string(1) "e"
string(13) " -1.000000e+0"
string(13) "-1.000000e+0 "
string(13) "	-1.000000e+0"
string(13) "
-1.000000e+0"
string(12) "-1.000000e+0"
string(30) "                  -1.000000e+0"
string(4) "0-1]"
string(1) "e"

-- Iteration 4 --
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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
string(11) "4.667000e+3"
string(1) "e"
string(11) "4.667000e+3"
string(1) "e"
string(12) " 4.667000e+3"
string(12) "4.667000e+3 "
string(12) "	4.667000e+3"
string(12) "
4.667000e+3"
string(11) "4.667000e+3"
string(30) "                   4.667000e+3"
string(4) "0-1]"
string(1) "e"

-- Iteration 9 --
string(11) "4.779000e+3"
string(1) "e"
string(11) "4.779000e+3"
string(1) "e"
string(12) " 4.779000e+3"
string(12) "4.779000e+3 "
string(12) "	4.779000e+3"
string(12) "
4.779000e+3"
string(11) "4.779000e+3"
string(30) "                   4.779000e+3"
string(4) "0-1]"
string(1) "e"

-- Iteration 10 --
string(11) "4.095000e+3"
string(1) "e"
string(11) "4.095000e+3"
string(1) "e"
string(12) " 4.095000e+3"
string(12) "4.095000e+3 "
string(12) "	4.095000e+3"
string(12) "
4.095000e+3"
string(11) "4.095000e+3"
string(30) "                   4.095000e+3"
string(4) "0-1]"
string(1) "e"

-- Iteration 11 --
string(11) "2.500000e+2"
string(1) "e"
string(11) "2.500000e+2"
string(1) "e"
string(12) " 2.500000e+2"
string(12) "2.500000e+2 "
string(12) "	2.500000e+2"
string(12) "
2.500000e+2"
string(11) "2.500000e+2"
string(30) "                   2.500000e+2"
string(4) "0-1]"
string(1) "e"

-- Iteration 12 --
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

-- Iteration 13 --
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

-- Iteration 14 --
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

-- Iteration 15 --
string(11) "8.300000e+1"
string(1) "e"
string(11) "8.300000e+1"
string(1) "e"
string(12) " 8.300000e+1"
string(12) "8.300000e+1 "
string(12) "	8.300000e+1"
string(12) "
8.300000e+1"
string(11) "8.300000e+1"
string(30) "                   8.300000e+1"
string(4) "0-1]"
string(1) "e"

-- Iteration 16 --
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

-- Iteration 17 --
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

-- Iteration 18 --
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
Done
