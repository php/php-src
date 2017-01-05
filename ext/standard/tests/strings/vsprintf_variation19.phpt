--TEST--
Test vsprintf() function : usage variations - with whitespaces in format strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vsprintf(string $format , array $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vsprintf() : with  white spaces in format strings ***\n";

// initializing the format array
$formats = array(
  "% d  %  d  %   d",
  "% f  %  f  %   f",
  "% F  %  F  %   F",
  "% b  %  b  %   b",
  "% c  %  c  %   c",
  "% e  %  e  %   e",
  "% u  %  u  %   u",
  "% o  %  o  %   o",
  "% x  %  x  %   x",
  "% X  %  X  %   X",
  "% E  %  E  %   E"
);

// initializing the args array

$args_array = array(
  array(111, 222, 333),
  array(1.1, .2, -0.6),
  array(1.12, -1.13, +0.23),
  array(1, 2, 3),
  array(65, 66, 67),
  array(2e1, 2e-1, -2e1),
  array(-11, +22, 33),
  array(012, -023, +023),
  array(0x11, -0x22, +0x33),
  array(0x11, -0x22, +0x33),
  array(2e1, 2e-1, -2e1)
);

$counter = 1;
foreach($formats as $format) {
  echo"\n-- Iteration $counter --\n";
  var_dump( vsprintf($format, $args_array[$counter-1]) );
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : with  white spaces in format strings ***

-- Iteration 1 --
string(13) "111  222  333"

-- Iteration 2 --
string(29) "1.100000  0.200000  -0.600000"

-- Iteration 3 --
string(29) "1.120000  -1.130000  0.230000"

-- Iteration 4 --
string(9) "1  10  11"

-- Iteration 5 --
string(7) "A  B  C"

-- Iteration 6 --
string(38) "2.000000e+1  2.000000e-1  -2.000000e+1"

-- Iteration 7 --
string(18) "4294967285  22  33"

-- Iteration 8 --
string(19) "12  37777777755  23"

-- Iteration 9 --
string(16) "11  ffffffde  33"

-- Iteration 10 --
string(16) "11  FFFFFFDE  33"

-- Iteration 11 --
string(38) "2.000000E+1  2.000000E-1  -2.000000E+1"
Done
