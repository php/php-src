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
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal 
  01912,       // should be quivalent to octal 1
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
--EXPECT--
*** Testing sprintf() : scientific formats with integer values ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 3 --
unicode(12) "-1.000000e+0"
unicode(1) "e"
unicode(12) "-1.000000e+0"
unicode(1) "e"
unicode(13) " -1.000000e+0"
unicode(13) "-1.000000e+0 "
unicode(13) "	-1.000000e+0"
unicode(13) "
-1.000000e+0"
unicode(12) "-1.000000e+0"
unicode(30) "                  -1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 4 --
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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
unicode(11) "4.667000e+3"
unicode(1) "e"
unicode(11) "4.667000e+3"
unicode(1) "e"
unicode(12) " 4.667000e+3"
unicode(12) "4.667000e+3 "
unicode(12) "	4.667000e+3"
unicode(12) "
4.667000e+3"
unicode(11) "4.667000e+3"
unicode(30) "                   4.667000e+3"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 9 --
unicode(11) "4.779000e+3"
unicode(1) "e"
unicode(11) "4.779000e+3"
unicode(1) "e"
unicode(12) " 4.779000e+3"
unicode(12) "4.779000e+3 "
unicode(12) "	4.779000e+3"
unicode(12) "
4.779000e+3"
unicode(11) "4.779000e+3"
unicode(30) "                   4.779000e+3"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 10 --
unicode(11) "4.095000e+3"
unicode(1) "e"
unicode(11) "4.095000e+3"
unicode(1) "e"
unicode(12) " 4.095000e+3"
unicode(12) "4.095000e+3 "
unicode(12) "	4.095000e+3"
unicode(12) "
4.095000e+3"
unicode(11) "4.095000e+3"
unicode(30) "                   4.095000e+3"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 11 --
unicode(11) "2.500000e+2"
unicode(1) "e"
unicode(11) "2.500000e+2"
unicode(1) "e"
unicode(12) " 2.500000e+2"
unicode(12) "2.500000e+2 "
unicode(12) "	2.500000e+2"
unicode(12) "
2.500000e+2"
unicode(11) "2.500000e+2"
unicode(30) "                   2.500000e+2"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 12 --
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

-- Iteration 13 --
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

-- Iteration 14 --
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

-- Iteration 15 --
unicode(11) "8.300000e+1"
unicode(1) "e"
unicode(11) "8.300000e+1"
unicode(1) "e"
unicode(12) " 8.300000e+1"
unicode(12) "8.300000e+1 "
unicode(12) "	8.300000e+1"
unicode(12) "
8.300000e+1"
unicode(11) "8.300000e+1"
unicode(30) "                   8.300000e+1"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 16 --
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

-- Iteration 17 --
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

-- Iteration 18 --
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
Done
