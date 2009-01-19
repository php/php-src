--TEST--
Test vprintf() function : usage variations - with whitespaces in format strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string vprintf(string $format , array $args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vprintf() : with  white spaces in format strings ***\n";

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
  array(012, -02394, +02389),
  array(0x11, -0x22, +0x33),
  array(0x11, -0x22, +0x33),
  array(2e1, 2e-1, -2e1)
);

$counter = 1;
foreach($formats as $format) {
  echo"\n-- Iteration $counter --\n";
  $result = vprintf($format, $args_array[$counter-1]);
  echo "\n";
  var_dump($result); 
  $counter++;
}

?>
===DONE===
--EXPECT--
*** Testing vprintf() : with  white spaces in format strings ***

-- Iteration 1 --
111  222  333
int(13)

-- Iteration 2 --
1.100000  0.200000  -0.600000
int(29)

-- Iteration 3 --
1.120000  -1.130000  0.230000
int(29)

-- Iteration 4 --
1  10  11
int(9)

-- Iteration 5 --
A  B  C
int(7)

-- Iteration 6 --
2.000000e+1  2.000000e-1  -2.000000e+1
int(38)

-- Iteration 7 --
18446744073709551605  22  33
int(28)

-- Iteration 8 --
12  1777777777777777777755  23
int(30)

-- Iteration 9 --
11  ffffffffffffffde  33
int(24)

-- Iteration 10 --
11  FFFFFFFFFFFFFFDE  33
int(24)

-- Iteration 11 --
2.000000E+1  2.000000E-1  -2.000000E+1
int(38)
===DONE===
