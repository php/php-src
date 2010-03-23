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
--EXPECTF--
*** Testing sprintf() : string formats with integer values ***

-- Iteration 1 --
string(1) "0"
string(1) "s"
string(1) "0"
string(1) "s"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 2 --
string(1) "1"
string(1) "s"
string(1) "1"
string(1) "s"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 3 --
string(2) "-1"
string(1) "s"
string(2) "-1"
string(1) "s"
string(3) " -1"
string(3) "-1 "
string(3) "	-1"
string(3) "
-1"
string(4) "  -1"
string(30) "                            -1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 4 --
string(11) "-2147483648"
string(1) "s"
string(11) "-2147483648"
string(1) "s"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 5 --
string(11) "-2147483647"
string(1) "s"
string(11) "-2147483647"
string(1) "s"
string(12) " -2147483647"
string(12) "-2147483647 "
string(12) "	-2147483647"
string(12) "
-2147483647"
string(11) "-2147483647"
string(30) "                   -2147483647"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 6 --
string(10) "2147483647"
string(1) "s"
string(10) "2147483647"
string(1) "s"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 7 --
string(10) "2147483640"
string(1) "s"
string(10) "2147483640"
string(1) "s"
string(11) " 2147483640"
string(11) "2147483640 "
string(11) "	2147483640"
string(11) "
2147483640"
string(10) "2147483640"
string(30) "                    2147483640"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 8 --
string(4) "4667"
string(1) "s"
string(4) "4667"
string(1) "s"
string(5) " 4667"
string(5) "4667 "
string(5) "	4667"
string(5) "
4667"
string(4) "4667"
string(30) "                          4667"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 9 --
string(4) "4779"
string(1) "s"
string(4) "4779"
string(1) "s"
string(5) " 4779"
string(5) "4779 "
string(5) "	4779"
string(5) "
4779"
string(4) "4779"
string(30) "                          4779"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 10 --
string(4) "4095"
string(1) "s"
string(4) "4095"
string(1) "s"
string(5) " 4095"
string(5) "4095 "
string(5) "	4095"
string(5) "
4095"
string(4) "4095"
string(30) "                          4095"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 11 --
string(3) "250"
string(1) "s"
string(3) "250"
string(1) "s"
string(4) " 250"
string(4) "250 "
string(4) "	250"
string(4) "
250"
string(4) " 250"
string(30) "                           250"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 12 --
string(11) "-2147483648"
string(1) "s"
string(11) "-2147483648"
string(1) "s"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 13 --
string(10) "2147483647"
string(1) "s"
string(10) "2147483647"
string(1) "s"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 14 --
string(10) "2147483647"
string(1) "s"
string(10) "2147483647"
string(1) "s"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 15 --
string(2) "83"
string(1) "s"
string(2) "83"
string(1) "s"
string(3) " 83"
string(3) "83 "
string(3) "	83"
string(3) "
83"
string(4) "  83"
string(30) "                            83"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 16 --
string(1) "1"
string(1) "s"
string(1) "1"
string(1) "s"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 17 --
string(11) "-2147483648"
string(1) "s"
string(11) "-2147483648"
string(1) "s"
string(12) " -2147483648"
string(12) "-2147483648 "
string(12) "	-2147483648"
string(12) "
-2147483648"
string(11) "-2147483648"
string(30) "                   -2147483648"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 18 --
string(10) "2147483647"
string(1) "s"
string(10) "2147483647"
string(1) "s"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(10) "a-zA-Z0-9]"
string(1) "s"
Done