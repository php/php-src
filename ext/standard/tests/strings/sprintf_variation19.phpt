--TEST--
Test sprintf() function : usage variations - string formats with integer values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : string formats with integer values ***\n";

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

// array of string formats
$string_formats = array( 
  "%s", "%hs", "%ls", 
  "%Ls"," %s", "%s ", 
  "\t%s", "\n%s", "%4s",
  "%30s", "%[a-zA-Z0-9]", "%*s"
);

$count = 1;
foreach($integer_values as $integer_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($string_formats as $format) {
    var_dump( sprintf($format, $integer_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : string formats with integer values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "s"
unicode(1) "0"
unicode(1) "s"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "s"
unicode(1) "1"
unicode(1) "s"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 3 --
unicode(2) "-1"
unicode(1) "s"
unicode(2) "-1"
unicode(1) "s"
unicode(3) " -1"
unicode(3) "-1 "
unicode(3) "	-1"
unicode(3) "
-1"
unicode(4) "  -1"
unicode(30) "                            -1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 4 --
unicode(11) "-2147483648"
unicode(1) "s"
unicode(11) "-2147483648"
unicode(1) "s"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 5 --
unicode(11) "-2147483647"
unicode(1) "s"
unicode(11) "-2147483647"
unicode(1) "s"
unicode(12) " -2147483647"
unicode(12) "-2147483647 "
unicode(12) "	-2147483647"
unicode(12) "
-2147483647"
unicode(11) "-2147483647"
unicode(30) "                   -2147483647"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 6 --
unicode(10) "2147483647"
unicode(1) "s"
unicode(10) "2147483647"
unicode(1) "s"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 7 --
unicode(10) "2147483640"
unicode(1) "s"
unicode(10) "2147483640"
unicode(1) "s"
unicode(11) " 2147483640"
unicode(11) "2147483640 "
unicode(11) "	2147483640"
unicode(11) "
2147483640"
unicode(10) "2147483640"
unicode(30) "                    2147483640"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 8 --
unicode(4) "4667"
unicode(1) "s"
unicode(4) "4667"
unicode(1) "s"
unicode(5) " 4667"
unicode(5) "4667 "
unicode(5) "	4667"
unicode(5) "
4667"
unicode(4) "4667"
unicode(30) "                          4667"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 9 --
unicode(4) "4779"
unicode(1) "s"
unicode(4) "4779"
unicode(1) "s"
unicode(5) " 4779"
unicode(5) "4779 "
unicode(5) "	4779"
unicode(5) "
4779"
unicode(4) "4779"
unicode(30) "                          4779"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 10 --
unicode(4) "4095"
unicode(1) "s"
unicode(4) "4095"
unicode(1) "s"
unicode(5) " 4095"
unicode(5) "4095 "
unicode(5) "	4095"
unicode(5) "
4095"
unicode(4) "4095"
unicode(30) "                          4095"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 11 --
unicode(3) "250"
unicode(1) "s"
unicode(3) "250"
unicode(1) "s"
unicode(4) " 250"
unicode(4) "250 "
unicode(4) "	250"
unicode(4) "
250"
unicode(4) " 250"
unicode(30) "                           250"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 12 --
unicode(11) "-2147483648"
unicode(1) "s"
unicode(11) "-2147483648"
unicode(1) "s"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 13 --
unicode(10) "2147483647"
unicode(1) "s"
unicode(10) "2147483647"
unicode(1) "s"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 14 --
unicode(10) "2147483647"
unicode(1) "s"
unicode(10) "2147483647"
unicode(1) "s"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 15 --
unicode(2) "83"
unicode(1) "s"
unicode(2) "83"
unicode(1) "s"
unicode(3) " 83"
unicode(3) "83 "
unicode(3) "	83"
unicode(3) "
83"
unicode(4) "  83"
unicode(30) "                            83"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 16 --
unicode(1) "1"
unicode(1) "s"
unicode(1) "1"
unicode(1) "s"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 17 --
unicode(11) "-2147483648"
unicode(1) "s"
unicode(11) "-2147483648"
unicode(1) "s"
unicode(12) " -2147483648"
unicode(12) "-2147483648 "
unicode(12) "	-2147483648"
unicode(12) "
-2147483648"
unicode(11) "-2147483648"
unicode(30) "                   -2147483648"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 18 --
unicode(10) "2147483647"
unicode(1) "s"
unicode(10) "2147483647"
unicode(1) "s"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"
Done