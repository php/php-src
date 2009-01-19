--TEST--
Test vprintf() function : usage variations - hexa formats with hexa values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different hexa formats and hexa values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : hexa formats with hexa values ***\n";

// defining array of different hexa formats
$formats = array(
  "%x",
  "%+x %-x %X",
  "%lx %Lx, %4x %-4x",
  "%10.4x %-10.4x %04x %04.4x",
  "%'#2x %'2x %'$2x %'_2x",
  "%x %x %x %x",
  "% %%x x%",
  '%3$x %4$x %1$x %2$x'
);

// Arrays of hexa values for the format defined in $format.
// Each sub array contains hexa values which correspond to each format string in $format
$args_array = array(
  array(0x0),
  array(-0x1, 0x1, +0x22),
  array(0x7FFFFFFF, -0x7fffffff, +0x7000000, -0x80000000),
  array(123456, 12345678, -1234567, 1234567),
  array(1, 0x2222, 0333333, -0x44444444),
  array(0x123b, 0xfAb, "0xaxz", 01293),
  array(0x1234, 0x34, 0x2ff),
  array(0x3, 0x4, 0x1, 0x2)

);

// looping to test vprintf() with different char octal from the above $format array
// and with octal values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";   
  $result = vprintf($format, $args_array[$counter-1]);
  echo "\n";
  var_dump($result);
  $counter++;
}

?>
===DONE===
--EXPECT--
*** Testing vprintf() : hexa formats with hexa values ***

-- Iteration 1 --
0
int(1)

-- Iteration 2 --
ffffffffffffffff 1 22
int(21)

-- Iteration 3 --
7fffffff x, 7000000 ffffffff80000000
int(36)

-- Iteration 4 --
                      ffffffffffed2979 0000
int(43)

-- Iteration 5 --
#1 2222 1b6db ffffffffbbbbbbbc
int(30)

-- Iteration 6 --
123b fab 0 a
int(12)

-- Iteration 7 --
%34 x
int(5)

-- Iteration 8 --
1 2 3 4
int(7)
===DONE===
