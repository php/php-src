--TEST--
Test sprintf() function : usage variations - hexa formats with integer values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
	    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : hexa formats with integer values ***\n";

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

// array of hexa formats
$hexa_formats = array(
  "%x", "%xx", "%lx",
  "%Lx", " %x", "%x ",
  "\t%x", "\n%x", "%4x",
  "%30x", "%[0-9A-Fa-f]", "%*x"
);

$count = 1;
foreach($integer_values as $integer_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($hexa_formats as $format) {
    var_dump( sprintf($format, $integer_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : hexa formats with integer values ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 3 --
string(8) "ffffffff"
string(9) "ffffffffx"
string(8) "ffffffff"
string(1) "x"
string(9) " ffffffff"
string(9) "ffffffff "
string(9) "	ffffffff"
string(9) "
ffffffff"
string(8) "ffffffff"
string(30) "                      ffffffff"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 4 --
string(8) "80000000"
string(9) "80000000x"
string(8) "80000000"
string(1) "x"
string(9) " 80000000"
string(9) "80000000 "
string(9) "	80000000"
string(9) "
80000000"
string(8) "80000000"
string(30) "                      80000000"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
string(8) "7ffffff8"
string(9) "7ffffff8x"
string(8) "7ffffff8"
string(1) "x"
string(9) " 7ffffff8"
string(9) "7ffffff8 "
string(9) "	7ffffff8"
string(9) "
7ffffff8"
string(8) "7ffffff8"
string(30) "                      7ffffff8"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 8 --
string(4) "123b"
string(5) "123bx"
string(4) "123b"
string(1) "x"
string(5) " 123b"
string(5) "123b "
string(5) "	123b"
string(5) "
123b"
string(4) "123b"
string(30) "                          123b"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 9 --
string(4) "12ab"
string(5) "12abx"
string(4) "12ab"
string(1) "x"
string(5) " 12ab"
string(5) "12ab "
string(5) "	12ab"
string(5) "
12ab"
string(4) "12ab"
string(30) "                          12ab"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 10 --
string(3) "fff"
string(4) "fffx"
string(3) "fff"
string(1) "x"
string(4) " fff"
string(4) "fff "
string(4) "	fff"
string(4) "
fff"
string(4) " fff"
string(30) "                           fff"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 11 --
string(2) "fa"
string(3) "fax"
string(2) "fa"
string(1) "x"
string(3) " fa"
string(3) "fa "
string(3) "	fa"
string(3) "
fa"
string(4) "  fa"
string(30) "                            fa"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 12 --
string(8) "80000000"
string(9) "80000000x"
string(8) "80000000"
string(1) "x"
string(9) " 80000000"
string(9) "80000000 "
string(9) "	80000000"
string(9) "
80000000"
string(8) "80000000"
string(30) "                      80000000"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 13 --
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

-- Iteration 14 --
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

-- Iteration 15 --
string(2) "53"
string(3) "53x"
string(2) "53"
string(1) "x"
string(3) " 53"
string(3) "53 "
string(3) "	53"
string(3) "
53"
string(4) "  53"
string(30) "                            53"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 16 --
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

-- Iteration 17 --
string(8) "80000000"
string(9) "80000000x"
string(8) "80000000"
string(1) "x"
string(9) " 80000000"
string(9) "80000000 "
string(9) "	80000000"
string(9) "
80000000"
string(8) "80000000"
string(30) "                      80000000"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 18 --
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
Done
