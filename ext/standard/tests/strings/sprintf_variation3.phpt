--TEST--
Test sprintf() function : usage variations - int formats with int values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : integer formats with integer values ***\n";

// different valid  integer values
$valid_ints = array(
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
// various integer formats
$int_formats = array(
  "%d", "%hd", "%ld",
  "%Ld", " %d", "%d ",
  "\t%d", "\n%d", "%4d",
  "%30d", "%[0-9]", "%*d"
);

$count = 1;
foreach($valid_ints as $int_value) {
  echo "\n-- Iteration $count --\n";

  foreach($int_formats as $format) {
    var_dump( sprintf($format, $int_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : integer formats with integer values ***

-- Iteration 1 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 2 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 3 --
string(2) "-1"
string(1) "d"
string(2) "-1"
string(1) "d"
string(3) " -1"
string(3) "-1 "
string(3) "	-1"
string(3) "
-1"
string(4) "  -1"
string(30) "                            -1"
string(4) "0-9]"
string(1) "d"

-- Iteration 4 --
string(11) "-2147483648"
string(1) "d"
string(11) "-2147483648"
string(1) "d"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(4) "0-9]"
string(1) "d"

-- Iteration 5 --
string(11) "-2147483647"
string(1) "d"
string(11) "-2147483647"
string(1) "d"
string(12) " -2147483647"
string(12) "-2147483647 "
string(12) "	-2147483647"
string(12) "
-2147483647"
string(11) "-2147483647"
string(30) "                   -2147483647"
string(4) "0-9]"
string(1) "d"

-- Iteration 6 --
string(10) "2147483647"
string(1) "d"
string(10) "2147483647"
string(1) "d"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "d"

-- Iteration 7 --
string(10) "2147483640"
string(1) "d"
string(10) "2147483640"
string(1) "d"
string(11) " 2147483640"
string(11) "2147483640 "
string(11) "	2147483640"
string(11) "
2147483640"
string(10) "2147483640"
string(30) "                    2147483640"
string(4) "0-9]"
string(1) "d"

-- Iteration 8 --
string(4) "4667"
string(1) "d"
string(4) "4667"
string(1) "d"
string(5) " 4667"
string(5) "4667 "
string(5) "	4667"
string(5) "
4667"
string(4) "4667"
string(30) "                          4667"
string(4) "0-9]"
string(1) "d"

-- Iteration 9 --
string(4) "4779"
string(1) "d"
string(4) "4779"
string(1) "d"
string(5) " 4779"
string(5) "4779 "
string(5) "	4779"
string(5) "
4779"
string(4) "4779"
string(30) "                          4779"
string(4) "0-9]"
string(1) "d"

-- Iteration 10 --
string(4) "4095"
string(1) "d"
string(4) "4095"
string(1) "d"
string(5) " 4095"
string(5) "4095 "
string(5) "	4095"
string(5) "
4095"
string(4) "4095"
string(30) "                          4095"
string(4) "0-9]"
string(1) "d"

-- Iteration 11 --
string(3) "250"
string(1) "d"
string(3) "250"
string(1) "d"
string(4) " 250"
string(4) "250 "
string(4) "	250"
string(4) "
250"
string(4) " 250"
string(30) "                           250"
string(4) "0-9]"
string(1) "d"

-- Iteration 12 --
string(11) "-2147483648"
string(1) "d"
string(11) "-2147483648"
string(1) "d"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(4) "0-9]"
string(1) "d"

-- Iteration 13 --
string(10) "2147483647"
string(1) "d"
string(10) "2147483647"
string(1) "d"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "d"

-- Iteration 14 --
string(10) "2147483647"
string(1) "d"
string(10) "2147483647"
string(1) "d"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "d"

-- Iteration 15 --
string(2) "83"
string(1) "d"
string(2) "83"
string(1) "d"
string(3) " 83"
string(3) "83 "
string(3) "	83"
string(3) "
83"
string(4) "  83"
string(30) "                            83"
string(4) "0-9]"
string(1) "d"

-- Iteration 16 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 17 --
string(11) "-2147483648"
string(1) "d"
string(11) "-2147483648"
string(1) "d"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(4) "0-9]"
string(1) "d"

-- Iteration 18 --
string(10) "2147483647"
string(1) "d"
string(10) "2147483647"
string(1) "d"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "d"
Done
