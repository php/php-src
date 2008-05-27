--TEST--
Test sprintf() function : usage variations - int formats with int values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : integer formats with integer values ***\n";

// different valid  integer vlaues
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
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal
  01912,       // should be quivalent to octal 1
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
unicode(1) "0"
unicode(1) "d"
unicode(1) "0"
unicode(1) "d"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 3 --
unicode(2) "-1"
unicode(1) "d"
unicode(2) "-1"
unicode(1) "d"
unicode(3) " -1"
unicode(3) "-1 "
unicode(3) "	-1"
unicode(3) "
-1"
unicode(4) "  -1"
unicode(30) "                            -1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 4 --
unicode(11) "-2147483648"
unicode(1) "d"
unicode(11) "-2147483648"
unicode(1) "d"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 5 --
unicode(11) "-2147483647"
unicode(1) "d"
unicode(11) "-2147483647"
unicode(1) "d"
unicode(12) " -2147483647"
unicode(12) "-2147483647 "
unicode(12) "	-2147483647"
unicode(12) "
-2147483647"
unicode(11) "-2147483647"
unicode(30) "                   -2147483647"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 6 --
unicode(10) "2147483647"
unicode(1) "d"
unicode(10) "2147483647"
unicode(1) "d"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 7 --
unicode(10) "2147483640"
unicode(1) "d"
unicode(10) "2147483640"
unicode(1) "d"
unicode(11) " 2147483640"
unicode(11) "2147483640 "
unicode(11) "	2147483640"
unicode(11) "
2147483640"
unicode(10) "2147483640"
unicode(30) "                    2147483640"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 8 --
unicode(4) "4667"
unicode(1) "d"
unicode(4) "4667"
unicode(1) "d"
unicode(5) " 4667"
unicode(5) "4667 "
unicode(5) "	4667"
unicode(5) "
4667"
unicode(4) "4667"
unicode(30) "                          4667"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 9 --
unicode(4) "4779"
unicode(1) "d"
unicode(4) "4779"
unicode(1) "d"
unicode(5) " 4779"
unicode(5) "4779 "
unicode(5) "	4779"
unicode(5) "
4779"
unicode(4) "4779"
unicode(30) "                          4779"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 10 --
unicode(4) "4095"
unicode(1) "d"
unicode(4) "4095"
unicode(1) "d"
unicode(5) " 4095"
unicode(5) "4095 "
unicode(5) "	4095"
unicode(5) "
4095"
unicode(4) "4095"
unicode(30) "                          4095"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 11 --
unicode(3) "250"
unicode(1) "d"
unicode(3) "250"
unicode(1) "d"
unicode(4) " 250"
unicode(4) "250 "
unicode(4) "	250"
unicode(4) "
250"
unicode(4) " 250"
unicode(30) "                           250"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 12 --
unicode(11) "-2147483648"
unicode(1) "d"
unicode(11) "-2147483648"
unicode(1) "d"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 13 --
unicode(10) "2147483647"
unicode(1) "d"
unicode(10) "2147483647"
unicode(1) "d"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 14 --
unicode(10) "2147483647"
unicode(1) "d"
unicode(10) "2147483647"
unicode(1) "d"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 15 --
unicode(2) "83"
unicode(1) "d"
unicode(2) "83"
unicode(1) "d"
unicode(3) " 83"
unicode(3) "83 "
unicode(3) "	83"
unicode(3) "
83"
unicode(4) "  83"
unicode(30) "                            83"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 16 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 17 --
unicode(11) "-2147483648"
unicode(1) "d"
unicode(11) "-2147483648"
unicode(1) "d"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 18 --
unicode(10) "2147483647"
unicode(1) "d"
unicode(10) "2147483647"
unicode(1) "d"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "d"
Done
