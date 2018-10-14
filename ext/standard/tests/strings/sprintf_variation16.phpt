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
--EXPECTF--
*** Testing sprintf() : string formats with float values ***

-- Iteration 1 --
string(11) "-2147483649"
string(1) "s"
string(11) "-2147483649"
string(1) "s"
string(12) " -2147483649"
string(12) "-2147483649 "
string(12) "	-2147483649"
string(12) "
-2147483649"
string(11) "-2147483649"
string(30) "                   -2147483649"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 2 --
string(10) "2147483648"
string(1) "s"
string(10) "2147483648"
string(1) "s"
string(11) " 2147483648"
string(11) "2147483648 "
string(11) "	2147483648"
string(11) "
2147483648"
string(10) "2147483648"
string(30) "                    2147483648"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 3 --
string(11) "-2147483649"
string(1) "s"
string(11) "-2147483649"
string(1) "s"
string(12) " -2147483649"
string(12) "-2147483649 "
string(12) "	-2147483649"
string(12) "
-2147483649"
string(11) "-2147483649"
string(30) "                   -2147483649"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 4 --
string(11) "34359738369"
string(1) "s"
string(11) "34359738369"
string(1) "s"
string(12) " 34359738369"
string(12) "34359738369 "
string(12) "	34359738369"
string(12) "
34359738369"
string(11) "34359738369"
string(30) "                   34359738369"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 5 --
string(10) "2147483649"
string(1) "s"
string(10) "2147483649"
string(1) "s"
string(11) " 2147483649"
string(11) "2147483649 "
string(11) "	2147483649"
string(11) "
2147483649"
string(10) "2147483649"
string(30) "                    2147483649"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 6 --
string(11) "-2147483649"
string(1) "s"
string(11) "-2147483649"
string(1) "s"
string(12) " -2147483649"
string(12) "-2147483649 "
string(12) "	-2147483649"
string(12) "
-2147483649"
string(11) "-2147483649"
string(30) "                   -2147483649"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 7 --
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

-- Iteration 8 --
string(4) "-0.1"
string(1) "s"
string(4) "-0.1"
string(1) "s"
string(5) " -0.1"
string(5) "-0.1 "
string(5) "	-0.1"
string(5) "
-0.1"
string(4) "-0.1"
string(30) "                          -0.1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 9 --
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

-- Iteration 10 --
string(4) "1000"
string(1) "s"
string(4) "1000"
string(1) "s"
string(5) " 1000"
string(5) "1000 "
string(5) "	1000"
string(5) "
1000"
string(4) "1000"
string(30) "                          1000"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 11 --
string(4) "-100"
string(1) "s"
string(4) "-100"
string(1) "s"
string(5) " -100"
string(5) "-100 "
string(5) "	-100"
string(5) "
-100"
string(4) "-100"
string(30) "                          -100"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 12 --
string(9) "123456000"
string(1) "s"
string(9) "123456000"
string(1) "s"
string(10) " 123456000"
string(10) "123456000 "
string(10) "	123456000"
string(10) "
123456000"
string(9) "123456000"
string(30) "                     123456000"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 13 --
string(11) "-1234567000"
string(1) "s"
string(11) "-1234567000"
string(1) "s"
string(12) " -1234567000"
string(12) "-1234567000 "
string(12) "	-1234567000"
string(12) "
-1234567000"
string(11) "-1234567000"
string(30) "                   -1234567000"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 14 --
string(2) "10"
string(1) "s"
string(2) "10"
string(1) "s"
string(3) " 10"
string(3) "10 "
string(3) "	10"
string(3) "
10"
string(4) "  10"
string(30) "                            10"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 15 --
string(7) "1012300"
string(1) "s"
string(7) "1012300"
string(1) "s"
string(8) " 1012300"
string(8) "1012300 "
string(8) "	1012300"
string(8) "
1012300"
string(7) "1012300"
string(30) "                       1012300"
string(10) "a-zA-Z0-9]"
string(1) "s"
Done
