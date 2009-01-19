--TEST--
Test vprintf() function : usage variations - char formats with char values
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
* Test vprintf() for char formats with an array of chars passed to the function
*/

echo "*** Testing vprintf() : char formats with char values ***\n";


// defining array of char formats
$formats = array(
  "%c",
  "%+c %-c %C",
  "%lc %Lc, %4c %-4c",
  "%10.4c %-10.4c %04c %04.4c",
  "%'#2c %'2c %'$2c %'_2c",
  "%c %c %c %c",
  "% %%c c%",
  '%3$c %4$c %1$c %2$c'
);

// Arrays of char values for the format defined in $format.
// Each sub array contains char values which correspond to each format string in $format
$args_array = array(
  array(0),
  array('c', 67, 68),
  array(' ', " ", -67, +67),
  array(97, -97, 98, +98),
  array(97, -97, 98, +98),
  array(0x123b, 0xfAb, 0123, 01293),
  array(38, -1234, 2345),
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
===DONE===
--EXPECTF--
*** Testing vprintf() : char formats with char values ***

-- Iteration 1 --
%c
int(1)

-- Iteration 2 --
%c C 
int(4)

-- Iteration 3 --
%c c, %s C
int(10)

-- Iteration 4 --
a %s b b
int(9)

-- Iteration 5 --
a %s b b
int(9)

-- Iteration 6 --
; %s S 

int(9)

-- Iteration 7 --
%. c
int(4)

-- Iteration 8 --
A B C D
int(7)
===DONE===
