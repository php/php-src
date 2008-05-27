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
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal 
  01912,       // should be quivalent to octal 1
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
--EXPECT--
*** Testing sprintf() : unsigned formats with integer values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "u"
unicode(1) "1"
unicode(1) "u"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 3 --
unicode(20) "18446744073709551615"
unicode(1) "u"
unicode(20) "18446744073709551615"
unicode(1) "u"
unicode(21) " 18446744073709551615"
unicode(21) "18446744073709551615 "
unicode(21) "	18446744073709551615"
unicode(21) "
18446744073709551615"
unicode(20) "18446744073709551615"
unicode(30) "          18446744073709551615"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 4 --
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(21) " 18446744071562067968"
unicode(21) "18446744071562067968 "
unicode(21) "	18446744071562067968"
unicode(21) "
18446744071562067968"
unicode(20) "18446744071562067968"
unicode(30) "          18446744071562067968"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 5 --
unicode(20) "18446744071562067969"
unicode(1) "u"
unicode(20) "18446744071562067969"
unicode(1) "u"
unicode(21) " 18446744071562067969"
unicode(21) "18446744071562067969 "
unicode(21) "	18446744071562067969"
unicode(21) "
18446744071562067969"
unicode(20) "18446744071562067969"
unicode(30) "          18446744071562067969"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 6 --
unicode(10) "2147483647"
unicode(1) "u"
unicode(10) "2147483647"
unicode(1) "u"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 7 --
unicode(10) "2147483640"
unicode(1) "u"
unicode(10) "2147483640"
unicode(1) "u"
unicode(11) " 2147483640"
unicode(11) "2147483640 "
unicode(11) "	2147483640"
unicode(11) "
2147483640"
unicode(10) "2147483640"
unicode(30) "                    2147483640"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 8 --
unicode(4) "4667"
unicode(1) "u"
unicode(4) "4667"
unicode(1) "u"
unicode(5) " 4667"
unicode(5) "4667 "
unicode(5) "	4667"
unicode(5) "
4667"
unicode(4) "4667"
unicode(30) "                          4667"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 9 --
unicode(4) "4779"
unicode(1) "u"
unicode(4) "4779"
unicode(1) "u"
unicode(5) " 4779"
unicode(5) "4779 "
unicode(5) "	4779"
unicode(5) "
4779"
unicode(4) "4779"
unicode(30) "                          4779"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 10 --
unicode(4) "4095"
unicode(1) "u"
unicode(4) "4095"
unicode(1) "u"
unicode(5) " 4095"
unicode(5) "4095 "
unicode(5) "	4095"
unicode(5) "
4095"
unicode(4) "4095"
unicode(30) "                          4095"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 11 --
unicode(3) "250"
unicode(1) "u"
unicode(3) "250"
unicode(1) "u"
unicode(4) " 250"
unicode(4) "250 "
unicode(4) "	250"
unicode(4) "
250"
unicode(4) " 250"
unicode(30) "                           250"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 12 --
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(21) " 18446744071562067968"
unicode(21) "18446744071562067968 "
unicode(21) "	18446744071562067968"
unicode(21) "
18446744071562067968"
unicode(20) "18446744071562067968"
unicode(30) "          18446744071562067968"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 13 --
unicode(10) "2147483647"
unicode(1) "u"
unicode(10) "2147483647"
unicode(1) "u"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 14 --
unicode(10) "2147483647"
unicode(1) "u"
unicode(10) "2147483647"
unicode(1) "u"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 15 --
unicode(2) "83"
unicode(1) "u"
unicode(2) "83"
unicode(1) "u"
unicode(3) " 83"
unicode(3) "83 "
unicode(3) "	83"
unicode(3) "
83"
unicode(4) "  83"
unicode(30) "                            83"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 16 --
unicode(1) "1"
unicode(1) "u"
unicode(1) "1"
unicode(1) "u"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 17 --
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(20) "18446744071562067968"
unicode(1) "u"
unicode(21) " 18446744071562067968"
unicode(21) "18446744071562067968 "
unicode(21) "	18446744071562067968"
unicode(21) "
18446744071562067968"
unicode(20) "18446744071562067968"
unicode(30) "          18446744071562067968"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 18 --
unicode(10) "2147483647"
unicode(1) "u"
unicode(10) "2147483647"
unicode(1) "u"
unicode(11) " 2147483647"
unicode(11) "2147483647 "
unicode(11) "	2147483647"
unicode(11) "
2147483647"
unicode(10) "2147483647"
unicode(30) "                    2147483647"
unicode(4) "0-9]"
unicode(1) "u"
Done
