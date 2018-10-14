--TEST--
Test sprintf() function : usage variations - unsigned formats with integer values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : unsigned formats with integer values ***\n";

// array of integer values
$integer_values = array(
  0,
  1,
  -1,
  -2147483648, // max negative integer value
  -2147483647,
  2147483647,  // max positive integer value
  +2147483640,
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

// array of unsigned formats
$unsigned_formats = array(
  "%u", "%hu", "%lu",
  "%Lu", " %u", "%u ",
  "\t%u", "\n%u", "%4u",
  "%30u", "%[0-9]", "%*u"
);


$count = 1;
foreach($integer_values as $integer_value) {
  echo "\n-- Iteration $count --\n";

  foreach($unsigned_formats as $format) {
    var_dump( sprintf($format, $integer_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : unsigned formats with integer values ***

-- Iteration 1 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 2 --
string(1) "1"
string(1) "u"
string(1) "1"
string(1) "u"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "u"

-- Iteration 3 --
string(20) "18446744073709551615"
string(1) "u"
string(20) "18446744073709551615"
string(1) "u"
string(21) " 18446744073709551615"
string(21) "18446744073709551615 "
string(21) "	18446744073709551615"
string(21) "
18446744073709551615"
string(20) "18446744073709551615"
string(30) "          18446744073709551615"
string(4) "0-9]"
string(1) "u"

-- Iteration 4 --
string(20) "18446744071562067968"
string(1) "u"
string(20) "18446744071562067968"
string(1) "u"
string(21) " 18446744071562067968"
string(21) "18446744071562067968 "
string(21) "	18446744071562067968"
string(21) "
18446744071562067968"
string(20) "18446744071562067968"
string(30) "          18446744071562067968"
string(4) "0-9]"
string(1) "u"

-- Iteration 5 --
string(20) "18446744071562067969"
string(1) "u"
string(20) "18446744071562067969"
string(1) "u"
string(21) " 18446744071562067969"
string(21) "18446744071562067969 "
string(21) "	18446744071562067969"
string(21) "
18446744071562067969"
string(20) "18446744071562067969"
string(30) "          18446744071562067969"
string(4) "0-9]"
string(1) "u"

-- Iteration 6 --
string(10) "2147483647"
string(1) "u"
string(10) "2147483647"
string(1) "u"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "u"

-- Iteration 7 --
string(10) "2147483640"
string(1) "u"
string(10) "2147483640"
string(1) "u"
string(11) " 2147483640"
string(11) "2147483640 "
string(11) "	2147483640"
string(11) "
2147483640"
string(10) "2147483640"
string(30) "                    2147483640"
string(4) "0-9]"
string(1) "u"

-- Iteration 8 --
string(4) "4667"
string(1) "u"
string(4) "4667"
string(1) "u"
string(5) " 4667"
string(5) "4667 "
string(5) "	4667"
string(5) "
4667"
string(4) "4667"
string(30) "                          4667"
string(4) "0-9]"
string(1) "u"

-- Iteration 9 --
string(4) "4779"
string(1) "u"
string(4) "4779"
string(1) "u"
string(5) " 4779"
string(5) "4779 "
string(5) "	4779"
string(5) "
4779"
string(4) "4779"
string(30) "                          4779"
string(4) "0-9]"
string(1) "u"

-- Iteration 10 --
string(4) "4095"
string(1) "u"
string(4) "4095"
string(1) "u"
string(5) " 4095"
string(5) "4095 "
string(5) "	4095"
string(5) "
4095"
string(4) "4095"
string(30) "                          4095"
string(4) "0-9]"
string(1) "u"

-- Iteration 11 --
string(3) "250"
string(1) "u"
string(3) "250"
string(1) "u"
string(4) " 250"
string(4) "250 "
string(4) "	250"
string(4) "
250"
string(4) " 250"
string(30) "                           250"
string(4) "0-9]"
string(1) "u"

-- Iteration 12 --
string(20) "18446744071562067968"
string(1) "u"
string(20) "18446744071562067968"
string(1) "u"
string(21) " 18446744071562067968"
string(21) "18446744071562067968 "
string(21) "	18446744071562067968"
string(21) "
18446744071562067968"
string(20) "18446744071562067968"
string(30) "          18446744071562067968"
string(4) "0-9]"
string(1) "u"

-- Iteration 13 --
string(10) "2147483647"
string(1) "u"
string(10) "2147483647"
string(1) "u"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "u"

-- Iteration 14 --
string(10) "2147483647"
string(1) "u"
string(10) "2147483647"
string(1) "u"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "u"

-- Iteration 15 --
string(2) "83"
string(1) "u"
string(2) "83"
string(1) "u"
string(3) " 83"
string(3) "83 "
string(3) "	83"
string(3) "
83"
string(4) "  83"
string(30) "                            83"
string(4) "0-9]"
string(1) "u"

-- Iteration 16 --
string(1) "1"
string(1) "u"
string(1) "1"
string(1) "u"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "u"

-- Iteration 17 --
string(20) "18446744071562067968"
string(1) "u"
string(20) "18446744071562067968"
string(1) "u"
string(21) " 18446744071562067968"
string(21) "18446744071562067968 "
string(21) "	18446744071562067968"
string(21) "
18446744071562067968"
string(20) "18446744071562067968"
string(30) "          18446744071562067968"
string(4) "0-9]"
string(1) "u"

-- Iteration 18 --
string(10) "2147483647"
string(1) "u"
string(10) "2147483647"
string(1) "u"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
string(4) "0-9]"
string(1) "u"
Done
