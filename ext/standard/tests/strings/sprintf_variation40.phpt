--TEST--
Test sprintf() function : usage variations - unsigned formats with integer values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
        die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
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
  "%u", "%lu", " %u", "%u ",
  "\t%u", "\n%u", "%4u", "%30u",
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
string(10) "4294967295"
string(10) "4294967295"
string(11) " 4294967295"
string(11) "4294967295 "
string(11) "	4294967295"
string(11) "
4294967295"
string(10) "4294967295"
string(30) "                    4294967295"

-- Iteration 4 --
string(10) "2147483648"
string(10) "2147483648"
string(11) " 2147483648"
string(11) "2147483648 "
string(11) "	2147483648"
string(11) "
2147483648"
string(10) "2147483648"
string(30) "                    2147483648"

-- Iteration 5 --
string(10) "2147483649"
string(10) "2147483649"
string(11) " 2147483649"
string(11) "2147483649 "
string(11) "	2147483649"
string(11) "
2147483649"
string(10) "2147483649"
string(30) "                    2147483649"

-- Iteration 6 --
string(10) "2147483647"
string(10) "2147483647"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"

-- Iteration 7 --
string(10) "2147483640"
string(10) "2147483640"
string(11) " 2147483640"
string(11) "2147483640 "
string(11) "	2147483640"
string(11) "
2147483640"
string(10) "2147483640"
string(30) "                    2147483640"

-- Iteration 8 --
string(4) "4667"
string(4) "4667"
string(5) " 4667"
string(5) "4667 "
string(5) "	4667"
string(5) "
4667"
string(4) "4667"
string(30) "                          4667"

-- Iteration 9 --
string(4) "4779"
string(4) "4779"
string(5) " 4779"
string(5) "4779 "
string(5) "	4779"
string(5) "
4779"
string(4) "4779"
string(30) "                          4779"

-- Iteration 10 --
string(4) "4095"
string(4) "4095"
string(5) " 4095"
string(5) "4095 "
string(5) "	4095"
string(5) "
4095"
string(4) "4095"
string(30) "                          4095"

-- Iteration 11 --
string(3) "250"
string(3) "250"
string(4) " 250"
string(4) "250 "
string(4) "	250"
string(4) "
250"
string(4) " 250"
string(30) "                           250"

-- Iteration 12 --
string(10) "2147483648"
string(10) "2147483648"
string(11) " 2147483648"
string(11) "2147483648 "
string(11) "	2147483648"
string(11) "
2147483648"
string(10) "2147483648"
string(30) "                    2147483648"

-- Iteration 13 --
string(10) "2147483647"
string(10) "2147483647"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"

-- Iteration 14 --
string(10) "2147483647"
string(10) "2147483647"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"

-- Iteration 15 --
string(2) "83"
string(2) "83"
string(3) " 83"
string(3) "83 "
string(3) "	83"
string(3) "
83"
string(4) "  83"
string(30) "                            83"

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
string(10) "2147483648"
string(10) "2147483648"
string(11) " 2147483648"
string(11) "2147483648 "
string(11) "	2147483648"
string(11) "
2147483648"
string(10) "2147483648"
string(30) "                    2147483648"

-- Iteration 18 --
string(10) "2147483647"
string(10) "2147483647"
string(11) " 2147483647"
string(11) "2147483647 "
string(11) "	2147483647"
string(11) "
2147483647"
string(10) "2147483647"
string(30) "                    2147483647"
Done
