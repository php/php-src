--TEST--
Test vprintf() function : usage variations - int formats with int values
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different int formats and int values are passed to
 * the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : int formats with int values ***\n";


// defining array of int formats
$formats = array(
  "%d",
  "%+d %-d %D",
  "%ld %Ld, %4d %-4d",
  "%10.4d %-10.4d %04d %04.4d",
  "%'#2d %'2d %'$2d %'_2d",
  "%d %d %d %d",
  "% %%d d%",
  '%3$d %4$d %1$d %2$d'
);

// Arrays of int values for the format defined in $format.
// Each sub array contains int values which correspond to each format string in $format
$args_array = array(
  array(0),
  array(-1, 1, +22),
  array(2147483647, -2147483648, +2147483640, -2147483640),
  array(123456, 12345678, -1234567, 1234567),
  array(111, 2222, 333333, 44444444),
  array(0x123b, 0xfAb, 0123, 012),
  array(1234, -5678, 2345),
  array(3, 4, 1, 2)

);

// looping to test vprintf() with different int formats from the above $format array
// and with int values from the above $args_array array
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
*** Testing vprintf() : int formats with int values ***

-- Iteration 1 --
0
int(1)

-- Iteration 2 --
-1 1 
int(5)

-- Iteration 3 --
2147483647 d, 2147483640 -2147483640
int(36)

-- Iteration 4 --
    123456 12345678   -1234567 1234567
int(38)

-- Iteration 5 --
111 2222 333333 44444444
int(24)

-- Iteration 6 --
4667 4011 83 10
int(15)

-- Iteration 7 --
%-5678 d
int(8)

-- Iteration 8 --
1 2 3 4
int(7)
===DONE===
