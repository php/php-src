--TEST--
Test vprintf() function : usage variations - char formats with char values
--FILE--
<?php

/*
* Test vprintf() for char formats with an array of chars passed to the function
*/

echo "*** Testing vprintf() : char formats with char values ***\n";


// defining array of char formats
$formats = array(
  "%c",
  "%+c %-c",
  "%lc %4c %-4c",
  "%10.4c %-10.4c %04c %04.4c",
  "%'#2c %'2c %'$2c %'_2c",
  "%c %c %c %c",
  "% %%c",
  '%3$c %4$c %1$c %2$c'
);

// Arrays of char values for the format defined in $format.
// Each sub array contains char values which correspond to each format string in $format
$args_array = array(
  array(0),
  array('c', 67),
  array(' ', -67, +67),
  array(97, -97, 98, +98),
  array(97, -97, 98, +98),
  array(0x123b, 0xfAb, 0123, 012),
  array(38, -1234),
  array(67, 68, 65, 66)

);

// looping to test vprintf() with different char formats from the above $format array
// and with char values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  echo "\n-- Iteration $counter --\n";
  $result = vprintf($format, $args_array[$counter-1]);
  echo "\n";
  var_dump($result);
  $counter++;
}

?>
--EXPECTF--
*** Testing vprintf() : char formats with char values ***

-- Iteration 1 --
%0
int(1)

-- Iteration 2 --
%0 C
int(3)

-- Iteration 3 --
%0 ½ C
int(5)

-- Iteration 4 --
a Ÿ b b
int(7)

-- Iteration 5 --
a Ÿ b b
int(7)

-- Iteration 6 --
; « S 

int(7)

-- Iteration 7 --
%.
int(2)

-- Iteration 8 --
A B C D
int(7)
