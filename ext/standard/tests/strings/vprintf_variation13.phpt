--TEST--
Test vprintf() function : usage variations - hexa formats with hexa values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
 * Test vprintf() when different hexa formats and hexa values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : hexa formats with hexa values ***\n";

// defining array of different hexa formats
$formats = array(
  "%x",
  "%+x %-x %X",
  "%lx %4x %-4x",
  "%10.4x %-10.4x %04x %04.4x",
  "%'#2x %'2x %'$2x %'_2x",
  "%x %x %x %x",
  "% %%x",
  '%3$x %4$x %1$x %2$x'
);

// Arrays of hexa values for the format defined in $format.
// Each sub array contains hexa values which correspond to each format string in $format
$args_array = array(
  array(0x0),
  array(-0x1, 0x1, +0x22),
  array(0x7FFFFFFF, +0x7000000, -0x80000000),
  array(123456, 12345678, -1234567, 1234567),
  array(1, 0x2222, 0333333, -0x44444444),
  array(0x123b, 0xfAb, "0xaxz", 012),
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
--EXPECT--
*** Testing vprintf() : hexa formats with hexa values ***

-- Iteration 1 --
0
int(1)

-- Iteration 2 --
ffffffff 1 22
int(13)

-- Iteration 3 --
7fffffff 7000000 80000000
int(25)

-- Iteration 4 --
                      ffed2979 0000
int(35)

-- Iteration 5 --
#1 2222 1b6db bbbbbbbc
int(22)

-- Iteration 6 --
123b fab 0 a
int(12)

-- Iteration 7 --
%34
int(3)

-- Iteration 8 --
1 2 3 4
int(7)
