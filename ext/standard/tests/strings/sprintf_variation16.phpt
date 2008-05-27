--TEST--
Test sprintf() function : usage variations - string formats with float values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : string formats with float values ***\n";

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
  1e3,
  -1e2,
  1.23456E8,
  -1.234567E9,
  10.0000000000000000005,
  10.123e+5
);

// array of string formats
$string_formats = array( 
  "%s", "%hs", "%ls", 
  "%Ls"," %s", "%s ", 
  "\t%s", "\n%s", "%4s",
  "%30s", "%[a-zA-Z0-9]", "%*s"
);

$count = 1;
foreach($float_values as $float_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($string_formats as $format) {
    var_dump( sprintf($format, $float_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : string formats with float values ***

-- Iteration 1 --
unicode(11) "-2147483649"
unicode(1) "s"
unicode(11) "-2147483649"
unicode(1) "s"
unicode(12) " -2147483649"
unicode(12) "-2147483649 "
unicode(12) "	-2147483649"
unicode(12) "
-2147483649"
unicode(11) "-2147483649"
unicode(30) "                   -2147483649"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 2 --
unicode(10) "2147483648"
unicode(1) "s"
unicode(10) "2147483648"
unicode(1) "s"
unicode(11) " 2147483648"
unicode(11) "2147483648 "
unicode(11) "	2147483648"
unicode(11) "
2147483648"
unicode(10) "2147483648"
unicode(30) "                    2147483648"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 3 --
unicode(11) "-2147483649"
unicode(1) "s"
unicode(11) "-2147483649"
unicode(1) "s"
unicode(12) " -2147483649"
unicode(12) "-2147483649 "
unicode(12) "	-2147483649"
unicode(12) "
-2147483649"
unicode(11) "-2147483649"
unicode(30) "                   -2147483649"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 4 --
unicode(11) "34359738369"
unicode(1) "s"
unicode(11) "34359738369"
unicode(1) "s"
unicode(12) " 34359738369"
unicode(12) "34359738369 "
unicode(12) "	34359738369"
unicode(12) "
34359738369"
unicode(11) "34359738369"
unicode(30) "                   34359738369"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 5 --
unicode(10) "2147483649"
unicode(1) "s"
unicode(10) "2147483649"
unicode(1) "s"
unicode(11) " 2147483649"
unicode(11) "2147483649 "
unicode(11) "	2147483649"
unicode(11) "
2147483649"
unicode(10) "2147483649"
unicode(30) "                    2147483649"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 6 --
unicode(11) "-2147483649"
unicode(1) "s"
unicode(11) "-2147483649"
unicode(1) "s"
unicode(12) " -2147483649"
unicode(12) "-2147483649 "
unicode(12) "	-2147483649"
unicode(12) "
-2147483649"
unicode(11) "-2147483649"
unicode(30) "                   -2147483649"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 7 --
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

-- Iteration 8 --
unicode(4) "-0.1"
unicode(1) "s"
unicode(4) "-0.1"
unicode(1) "s"
unicode(5) " -0.1"
unicode(5) "-0.1 "
unicode(5) "	-0.1"
unicode(5) "
-0.1"
unicode(4) "-0.1"
unicode(30) "                          -0.1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 9 --
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

-- Iteration 10 --
unicode(4) "1000"
unicode(1) "s"
unicode(4) "1000"
unicode(1) "s"
unicode(5) " 1000"
unicode(5) "1000 "
unicode(5) "	1000"
unicode(5) "
1000"
unicode(4) "1000"
unicode(30) "                          1000"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 11 --
unicode(4) "-100"
unicode(1) "s"
unicode(4) "-100"
unicode(1) "s"
unicode(5) " -100"
unicode(5) "-100 "
unicode(5) "	-100"
unicode(5) "
-100"
unicode(4) "-100"
unicode(30) "                          -100"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 12 --
unicode(9) "123456000"
unicode(1) "s"
unicode(9) "123456000"
unicode(1) "s"
unicode(10) " 123456000"
unicode(10) "123456000 "
unicode(10) "	123456000"
unicode(10) "
123456000"
unicode(9) "123456000"
unicode(30) "                     123456000"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 13 --
unicode(11) "-1234567000"
unicode(1) "s"
unicode(11) "-1234567000"
unicode(1) "s"
unicode(12) " -1234567000"
unicode(12) "-1234567000 "
unicode(12) "	-1234567000"
unicode(12) "
-1234567000"
unicode(11) "-1234567000"
unicode(30) "                   -1234567000"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 14 --
unicode(2) "10"
unicode(1) "s"
unicode(2) "10"
unicode(1) "s"
unicode(3) " 10"
unicode(3) "10 "
unicode(3) "	10"
unicode(3) "
10"
unicode(4) "  10"
unicode(30) "                            10"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 15 --
unicode(7) "1012300"
unicode(1) "s"
unicode(7) "1012300"
unicode(1) "s"
unicode(8) " 1012300"
unicode(8) "1012300 "
unicode(8) "	1012300"
unicode(8) "
1012300"
unicode(7) "1012300"
unicode(30) "                       1012300"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"
Done
