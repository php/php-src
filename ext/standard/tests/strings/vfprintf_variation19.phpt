--TEST--
Test vfprintf() function : usage variations - with whitespaces in format strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing vfprintf() : with  white spaces in format strings ***\n";

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


/* creating dumping file */
$data_file = dirname(__FILE__) . '/vfprintf_variation19.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;
   
// looping to test vfprintf() with different scientific formats from the above $format array
// and with non-scientific values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
  fprintf($fp, "\n-- Iteration %d --\n",$counter);
  vfprintf($fp,$format, $args_array[$counter-1]);
  $counter++;
}

fclose($fp);
print_r(file_get_contents($data_file));
echo "\n";

unlink($data_file); 
?>
===DONE===
--EXPECT--
*** Testing vfprintf() : with  white spaces in format strings ***

-- Iteration 1 --
111  222  333
-- Iteration 2 --
1.100000  0.200000  -0.600000
-- Iteration 3 --
1.120000  -1.130000  0.230000
-- Iteration 4 --
1  10  11
-- Iteration 5 --
A  B  C
-- Iteration 6 --
2.000000e+1  2.000000e-1  -2.000000e+1
-- Iteration 7 --
4294967285  22  33
-- Iteration 8 --
12  37777777755  23
-- Iteration 9 --
11  ffffffde  33
-- Iteration 10 --
11  FFFFFFDE  33
-- Iteration 11 --
2.000000E+1  2.000000E-1  -2.000000E+1
===DONE===
