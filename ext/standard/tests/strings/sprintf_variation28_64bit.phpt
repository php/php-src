--TEST--
Test sprintf() function : usage variations - octal formats with integer values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with integer values ***\n";

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

// array of octal formats
$octal_formats = array(
  "%o", "%ho", "%lo",
  "%Lo", " %o", "%o ",
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($integer_values as $integer_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $integer_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : octal formats with integer values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "o"
unicode(1) "1"
unicode(1) "o"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 3 --
unicode(22) "1777777777777777777777"
unicode(1) "o"
unicode(22) "1777777777777777777777"
unicode(1) "o"
unicode(23) " 1777777777777777777777"
unicode(23) "1777777777777777777777 "
unicode(23) "	1777777777777777777777"
unicode(23) "
1777777777777777777777"
unicode(22) "1777777777777777777777"
unicode(30) "        1777777777777777777777"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 4 --
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(23) " 1777777777760000000000"
unicode(23) "1777777777760000000000 "
unicode(23) "	1777777777760000000000"
unicode(23) "
1777777777760000000000"
unicode(22) "1777777777760000000000"
unicode(30) "        1777777777760000000000"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 5 --
unicode(22) "1777777777760000000001"
unicode(1) "o"
unicode(22) "1777777777760000000001"
unicode(1) "o"
unicode(23) " 1777777777760000000001"
unicode(23) "1777777777760000000001 "
unicode(23) "	1777777777760000000001"
unicode(23) "
1777777777760000000001"
unicode(22) "1777777777760000000001"
unicode(30) "        1777777777760000000001"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 6 --
unicode(11) "17777777777"
unicode(1) "o"
unicode(11) "17777777777"
unicode(1) "o"
unicode(12) " 17777777777"
unicode(12) "17777777777 "
unicode(12) "	17777777777"
unicode(12) "
17777777777"
unicode(11) "17777777777"
unicode(30) "                   17777777777"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 7 --
unicode(11) "17777777770"
unicode(1) "o"
unicode(11) "17777777770"
unicode(1) "o"
unicode(12) " 17777777770"
unicode(12) "17777777770 "
unicode(12) "	17777777770"
unicode(12) "
17777777770"
unicode(11) "17777777770"
unicode(30) "                   17777777770"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 8 --
unicode(5) "11073"
unicode(1) "o"
unicode(5) "11073"
unicode(1) "o"
unicode(6) " 11073"
unicode(6) "11073 "
unicode(6) "	11073"
unicode(6) "
11073"
unicode(5) "11073"
unicode(30) "                         11073"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 9 --
unicode(5) "11253"
unicode(1) "o"
unicode(5) "11253"
unicode(1) "o"
unicode(6) " 11253"
unicode(6) "11253 "
unicode(6) "	11253"
unicode(6) "
11253"
unicode(5) "11253"
unicode(30) "                         11253"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 10 --
unicode(4) "7777"
unicode(1) "o"
unicode(4) "7777"
unicode(1) "o"
unicode(5) " 7777"
unicode(5) "7777 "
unicode(5) "	7777"
unicode(5) "
7777"
unicode(4) "7777"
unicode(30) "                          7777"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 11 --
unicode(3) "372"
unicode(1) "o"
unicode(3) "372"
unicode(1) "o"
unicode(4) " 372"
unicode(4) "372 "
unicode(4) "	372"
unicode(4) "
372"
unicode(4) " 372"
unicode(30) "                           372"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 12 --
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(23) " 1777777777760000000000"
unicode(23) "1777777777760000000000 "
unicode(23) "	1777777777760000000000"
unicode(23) "
1777777777760000000000"
unicode(22) "1777777777760000000000"
unicode(30) "        1777777777760000000000"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 13 --
unicode(11) "17777777777"
unicode(1) "o"
unicode(11) "17777777777"
unicode(1) "o"
unicode(12) " 17777777777"
unicode(12) "17777777777 "
unicode(12) "	17777777777"
unicode(12) "
17777777777"
unicode(11) "17777777777"
unicode(30) "                   17777777777"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 14 --
unicode(11) "17777777777"
unicode(1) "o"
unicode(11) "17777777777"
unicode(1) "o"
unicode(12) " 17777777777"
unicode(12) "17777777777 "
unicode(12) "	17777777777"
unicode(12) "
17777777777"
unicode(11) "17777777777"
unicode(30) "                   17777777777"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 15 --
unicode(3) "123"
unicode(1) "o"
unicode(3) "123"
unicode(1) "o"
unicode(4) " 123"
unicode(4) "123 "
unicode(4) "	123"
unicode(4) "
123"
unicode(4) " 123"
unicode(30) "                           123"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 16 --
unicode(1) "1"
unicode(1) "o"
unicode(1) "1"
unicode(1) "o"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 17 --
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(22) "1777777777760000000000"
unicode(1) "o"
unicode(23) " 1777777777760000000000"
unicode(23) "1777777777760000000000 "
unicode(23) "	1777777777760000000000"
unicode(23) "
1777777777760000000000"
unicode(22) "1777777777760000000000"
unicode(30) "        1777777777760000000000"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 18 --
unicode(11) "17777777777"
unicode(1) "o"
unicode(11) "17777777777"
unicode(1) "o"
unicode(12) " 17777777777"
unicode(12) "17777777777 "
unicode(12) "	17777777777"
unicode(12) "
17777777777"
unicode(11) "17777777777"
unicode(30) "                   17777777777"
unicode(4) "0-7]"
unicode(1) "o"
Done
