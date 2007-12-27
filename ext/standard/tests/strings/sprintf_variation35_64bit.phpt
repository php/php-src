--TEST--
Test sprintf() function : usage variations - hexa formats with float values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : hexa formats with float values ***\n";

// array of float values 
$float_values = array(
  2147483647,
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e6,
  1E8,
  -1E9,
  10.0000000000000000005,
  10.5e+5
);

// array of hexa formats
$hexa_formats = array(  
  "%x", "%xx", "%lx", 
  "%Lx", " %x", "%x ",
  "\t%x", "\n%x", "%4x",
  "%30x", "%[0-9A-Fa-f]", "%*x"
);

$count = 1;
foreach($float_values as $float_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($hexa_formats as $format) {
    // with two arguments
    var_dump( sprintf($format, $float_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : hexa formats with float values ***

-- Iteration 1 --
string(8) "7fffffff"
string(9) "7fffffffx"
string(8) "7fffffff"
string(1) "x"
string(9) " 7fffffff"
string(9) "7fffffff "
string(9) "	7fffffff"
string(9) "
7fffffff"
string(8) "7fffffff"
string(30) "                      7fffffff"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 2 --
string(9) "800000001"
string(10) "800000001x"
string(9) "800000001"
string(1) "x"
string(10) " 800000001"
string(10) "800000001 "
string(10) "	800000001"
string(10) "
800000001"
string(9) "800000001"
string(30) "                     800000001"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 3 --
string(8) "80000001"
string(9) "80000001x"
string(8) "80000001"
string(1) "x"
string(9) " 80000001"
string(9) "80000001 "
string(9) "	80000001"
string(9) "
80000001"
string(8) "80000001"
string(30) "                      80000001"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 4 --
string(1) "0"
string(2) "0x"
string(1) "0"
string(1) "x"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 5 --
string(1) "0"
string(2) "0x"
string(1) "0"
string(1) "x"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 6 --
string(1) "1"
string(2) "1x"
string(1) "1"
string(1) "x"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 7 --
string(5) "186a0"
string(6) "186a0x"
string(5) "186a0"
string(1) "x"
string(6) " 186a0"
string(6) "186a0 "
string(6) "	186a0"
string(6) "
186a0"
string(5) "186a0"
string(30) "                         186a0"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 8 --
string(16) "fffffffffff0bdc0"
string(17) "fffffffffff0bdc0x"
string(16) "fffffffffff0bdc0"
string(1) "x"
string(17) " fffffffffff0bdc0"
string(17) "fffffffffff0bdc0 "
string(17) "	fffffffffff0bdc0"
string(17) "
fffffffffff0bdc0"
string(16) "fffffffffff0bdc0"
string(30) "              fffffffffff0bdc0"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 9 --
string(7) "5f5e100"
string(8) "5f5e100x"
string(7) "5f5e100"
string(1) "x"
string(8) " 5f5e100"
string(8) "5f5e100 "
string(8) "	5f5e100"
string(8) "
5f5e100"
string(7) "5f5e100"
string(30) "                       5f5e100"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 10 --
string(16) "ffffffffc4653600"
string(17) "ffffffffc4653600x"
string(16) "ffffffffc4653600"
string(1) "x"
string(17) " ffffffffc4653600"
string(17) "ffffffffc4653600 "
string(17) "	ffffffffc4653600"
string(17) "
ffffffffc4653600"
string(16) "ffffffffc4653600"
string(30) "              ffffffffc4653600"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 11 --
string(1) "a"
string(2) "ax"
string(1) "a"
string(1) "x"
string(2) " a"
string(2) "a "
string(2) "	a"
string(2) "
a"
string(4) "   a"
string(30) "                             a"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 12 --
string(6) "100590"
string(7) "100590x"
string(6) "100590"
string(1) "x"
string(7) " 100590"
string(7) "100590 "
string(7) "	100590"
string(7) "
100590"
string(6) "100590"
string(30) "                        100590"
string(10) "0-9A-Fa-f]"
string(1) "x"
Done
