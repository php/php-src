--TEST--
Test sprintf() function : usage variations - hexa formats with integer values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
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
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal 
  01912,       // should be quivalent to octal 1
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
string(16) "ffffffffffffffff"
string(17) "ffffffffffffffffx"
string(16) "ffffffffffffffff"
string(1) "x"
string(17) " ffffffffffffffff"
string(17) "ffffffffffffffff "
string(17) "	ffffffffffffffff"
string(17) "
ffffffffffffffff"
string(16) "ffffffffffffffff"
string(30) "              ffffffffffffffff"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 4 --
string(16) "ffffffff80000000"
string(17) "ffffffff80000000x"
string(16) "ffffffff80000000"
string(1) "x"
string(17) " ffffffff80000000"
string(17) "ffffffff80000000 "
string(17) "	ffffffff80000000"
string(17) "
ffffffff80000000"
string(16) "ffffffff80000000"
string(30) "              ffffffff80000000"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 5 --
string(16) "ffffffff80000001"
string(17) "ffffffff80000001x"
string(16) "ffffffff80000001"
string(1) "x"
string(17) " ffffffff80000001"
string(17) "ffffffff80000001 "
string(17) "	ffffffff80000001"
string(17) "
ffffffff80000001"
string(16) "ffffffff80000001"
string(30) "              ffffffff80000001"
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
string(16) "ffffffff80000000"
string(17) "ffffffff80000000x"
string(16) "ffffffff80000000"
string(1) "x"
string(17) " ffffffff80000000"
string(17) "ffffffff80000000 "
string(17) "	ffffffff80000000"
string(17) "
ffffffff80000000"
string(16) "ffffffff80000000"
string(30) "              ffffffff80000000"
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
string(16) "ffffffff80000000"
string(17) "ffffffff80000000x"
string(16) "ffffffff80000000"
string(1) "x"
string(17) " ffffffff80000000"
string(17) "ffffffff80000000 "
string(17) "	ffffffff80000000"
string(17) "
ffffffff80000000"
string(16) "ffffffff80000000"
string(30) "              ffffffff80000000"
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

--UEXPECTF--
*** Testing sprintf() : hexa formats with integer values ***

-- Iteration 1 --
unicode(1) "0"
unicode(2) "0x"
unicode(1) "0"
unicode(1) "x"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 2 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 3 --
unicode(16) "ffffffffffffffff"
unicode(17) "ffffffffffffffffx"
unicode(16) "ffffffffffffffff"
unicode(1) "x"
unicode(17) " ffffffffffffffff"
unicode(17) "ffffffffffffffff "
unicode(17) "	ffffffffffffffff"
unicode(17) "
ffffffffffffffff"
unicode(16) "ffffffffffffffff"
unicode(30) "              ffffffffffffffff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 4 --
unicode(16) "ffffffff80000000"
unicode(17) "ffffffff80000000x"
unicode(16) "ffffffff80000000"
unicode(1) "x"
unicode(17) " ffffffff80000000"
unicode(17) "ffffffff80000000 "
unicode(17) "	ffffffff80000000"
unicode(17) "
ffffffff80000000"
unicode(16) "ffffffff80000000"
unicode(30) "              ffffffff80000000"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 5 --
unicode(16) "ffffffff80000001"
unicode(17) "ffffffff80000001x"
unicode(16) "ffffffff80000001"
unicode(1) "x"
unicode(17) " ffffffff80000001"
unicode(17) "ffffffff80000001 "
unicode(17) "	ffffffff80000001"
unicode(17) "
ffffffff80000001"
unicode(16) "ffffffff80000001"
unicode(30) "              ffffffff80000001"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 6 --
unicode(8) "7fffffff"
unicode(9) "7fffffffx"
unicode(8) "7fffffff"
unicode(1) "x"
unicode(9) " 7fffffff"
unicode(9) "7fffffff "
unicode(9) "	7fffffff"
unicode(9) "
7fffffff"
unicode(8) "7fffffff"
unicode(30) "                      7fffffff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 7 --
unicode(8) "7ffffff8"
unicode(9) "7ffffff8x"
unicode(8) "7ffffff8"
unicode(1) "x"
unicode(9) " 7ffffff8"
unicode(9) "7ffffff8 "
unicode(9) "	7ffffff8"
unicode(9) "
7ffffff8"
unicode(8) "7ffffff8"
unicode(30) "                      7ffffff8"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 8 --
unicode(4) "123b"
unicode(5) "123bx"
unicode(4) "123b"
unicode(1) "x"
unicode(5) " 123b"
unicode(5) "123b "
unicode(5) "	123b"
unicode(5) "
123b"
unicode(4) "123b"
unicode(30) "                          123b"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 9 --
unicode(4) "12ab"
unicode(5) "12abx"
unicode(4) "12ab"
unicode(1) "x"
unicode(5) " 12ab"
unicode(5) "12ab "
unicode(5) "	12ab"
unicode(5) "
12ab"
unicode(4) "12ab"
unicode(30) "                          12ab"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 10 --
unicode(3) "fff"
unicode(4) "fffx"
unicode(3) "fff"
unicode(1) "x"
unicode(4) " fff"
unicode(4) "fff "
unicode(4) "	fff"
unicode(4) "
fff"
unicode(4) " fff"
unicode(30) "                           fff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 11 --
unicode(2) "fa"
unicode(3) "fax"
unicode(2) "fa"
unicode(1) "x"
unicode(3) " fa"
unicode(3) "fa "
unicode(3) "	fa"
unicode(3) "
fa"
unicode(4) "  fa"
unicode(30) "                            fa"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 12 --
unicode(16) "ffffffff80000000"
unicode(17) "ffffffff80000000x"
unicode(16) "ffffffff80000000"
unicode(1) "x"
unicode(17) " ffffffff80000000"
unicode(17) "ffffffff80000000 "
unicode(17) "	ffffffff80000000"
unicode(17) "
ffffffff80000000"
unicode(16) "ffffffff80000000"
unicode(30) "              ffffffff80000000"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 13 --
unicode(8) "7fffffff"
unicode(9) "7fffffffx"
unicode(8) "7fffffff"
unicode(1) "x"
unicode(9) " 7fffffff"
unicode(9) "7fffffff "
unicode(9) "	7fffffff"
unicode(9) "
7fffffff"
unicode(8) "7fffffff"
unicode(30) "                      7fffffff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 14 --
unicode(8) "7fffffff"
unicode(9) "7fffffffx"
unicode(8) "7fffffff"
unicode(1) "x"
unicode(9) " 7fffffff"
unicode(9) "7fffffff "
unicode(9) "	7fffffff"
unicode(9) "
7fffffff"
unicode(8) "7fffffff"
unicode(30) "                      7fffffff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 15 --
unicode(2) "53"
unicode(3) "53x"
unicode(2) "53"
unicode(1) "x"
unicode(3) " 53"
unicode(3) "53 "
unicode(3) "	53"
unicode(3) "
53"
unicode(4) "  53"
unicode(30) "                            53"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 16 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 17 --
unicode(16) "ffffffff80000000"
unicode(17) "ffffffff80000000x"
unicode(16) "ffffffff80000000"
unicode(1) "x"
unicode(17) " ffffffff80000000"
unicode(17) "ffffffff80000000 "
unicode(17) "	ffffffff80000000"
unicode(17) "
ffffffff80000000"
unicode(16) "ffffffff80000000"
unicode(30) "              ffffffff80000000"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 18 --
unicode(8) "7fffffff"
unicode(9) "7fffffffx"
unicode(8) "7fffffff"
unicode(1) "x"
unicode(9) " 7fffffff"
unicode(9) "7fffffff "
unicode(9) "	7fffffff"
unicode(9) "
7fffffff"
unicode(8) "7fffffff"
unicode(30) "                      7fffffff"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"
Done
