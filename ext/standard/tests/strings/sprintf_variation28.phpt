--TEST--
Test sprintf() function : usage variations - octal formats with integer values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
        die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
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
  0x7fffffff,  // max positive integer as hexadecimal
  0x7FFFFFFF,  // max positive integer as hexadecimal
  0123,        // integer as octal
  01,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal
  017777777777  // max positive integer as octal
);

// array of octal formats
$octal_formats = array(
  "%o", "%lo", " %o", "%o ",
  "\t%o", "\n%o", "%4o", "%30o",
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
string(1) "0"
string(1) "0"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"

-- Iteration 2 --
string(1) "1"
string(1) "1"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"

-- Iteration 3 --
string(11) "37777777777"
string(11) "37777777777"
string(12) " 37777777777"
string(12) "37777777777 "
string(12) "	37777777777"
string(12) "
37777777777"
string(11) "37777777777"
string(30) "                   37777777777"

-- Iteration 4 --
string(11) "20000000000"
string(11) "20000000000"
string(12) " 20000000000"
string(12) "20000000000 "
string(12) "	20000000000"
string(12) "
20000000000"
string(11) "20000000000"
string(30) "                   20000000000"

-- Iteration 5 --
string(11) "20000000001"
string(11) "20000000001"
string(12) " 20000000001"
string(12) "20000000001 "
string(12) "	20000000001"
string(12) "
20000000001"
string(11) "20000000001"
string(30) "                   20000000001"

-- Iteration 6 --
string(11) "17777777777"
string(11) "17777777777"
string(12) " 17777777777"
string(12) "17777777777 "
string(12) "	17777777777"
string(12) "
17777777777"
string(11) "17777777777"
string(30) "                   17777777777"

-- Iteration 7 --
string(11) "17777777770"
string(11) "17777777770"
string(12) " 17777777770"
string(12) "17777777770 "
string(12) "	17777777770"
string(12) "
17777777770"
string(11) "17777777770"
string(30) "                   17777777770"

-- Iteration 8 --
string(5) "11073"
string(5) "11073"
string(6) " 11073"
string(6) "11073 "
string(6) "	11073"
string(6) "
11073"
string(5) "11073"
string(30) "                         11073"

-- Iteration 9 --
string(5) "11253"
string(5) "11253"
string(6) " 11253"
string(6) "11253 "
string(6) "	11253"
string(6) "
11253"
string(5) "11253"
string(30) "                         11253"

-- Iteration 10 --
string(4) "7777"
string(4) "7777"
string(5) " 7777"
string(5) "7777 "
string(5) "	7777"
string(5) "
7777"
string(4) "7777"
string(30) "                          7777"

-- Iteration 11 --
string(3) "372"
string(3) "372"
string(4) " 372"
string(4) "372 "
string(4) "	372"
string(4) "
372"
string(4) " 372"
string(30) "                           372"

-- Iteration 12 --
string(11) "20000000000"
string(11) "20000000000"
string(12) " 20000000000"
string(12) "20000000000 "
string(12) "	20000000000"
string(12) "
20000000000"
string(11) "20000000000"
string(30) "                   20000000000"

-- Iteration 13 --
string(11) "17777777777"
string(11) "17777777777"
string(12) " 17777777777"
string(12) "17777777777 "
string(12) "	17777777777"
string(12) "
17777777777"
string(11) "17777777777"
string(30) "                   17777777777"

-- Iteration 14 --
string(11) "17777777777"
string(11) "17777777777"
string(12) " 17777777777"
string(12) "17777777777 "
string(12) "	17777777777"
string(12) "
17777777777"
string(11) "17777777777"
string(30) "                   17777777777"

-- Iteration 15 --
string(3) "123"
string(3) "123"
string(4) " 123"
string(4) "123 "
string(4) "	123"
string(4) "
123"
string(4) " 123"
string(30) "                           123"

-- Iteration 16 --
string(1) "1"
string(1) "1"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"

-- Iteration 17 --
string(11) "20000000000"
string(11) "20000000000"
string(12) " 20000000000"
string(12) "20000000000 "
string(12) "	20000000000"
string(12) "
20000000000"
string(11) "20000000000"
string(30) "                   20000000000"

-- Iteration 18 --
string(11) "17777777777"
string(11) "17777777777"
string(12) " 17777777777"
string(12) "17777777777 "
string(12) "	17777777777"
string(12) "
17777777777"
string(11) "17777777777"
string(30) "                   17777777777"
Done
