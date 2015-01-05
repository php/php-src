--TEST--
Test sprintf() function : usage variations - octal formats with float values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with float values ***\n";

// array of float values 
$float_values = array(
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e6,
  1E8,
  -1E9,
  10.5e+5
);

// array of octal formats
$octal_formats = array( 
  "%o", "%ho", "%lo", 
  "%Lo", " %o", "%o ",                        
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($float_values as $float_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $float_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : octal formats with float values ***

-- Iteration 1 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 2 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 3 --
string(1) "1"
string(1) "o"
string(1) "1"
string(1) "o"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-7]"
string(1) "o"

-- Iteration 4 --
string(6) "303240"
string(1) "o"
string(6) "303240"
string(1) "o"
string(7) " 303240"
string(7) "303240 "
string(7) "	303240"
string(7) "
303240"
string(6) "303240"
string(30) "                        303240"
string(4) "0-7]"
string(1) "o"

-- Iteration 5 --
string(22) "1777777777777774136700"
string(1) "o"
string(22) "1777777777777774136700"
string(1) "o"
string(23) " 1777777777777774136700"
string(23) "1777777777777774136700 "
string(23) "	1777777777777774136700"
string(23) "
1777777777777774136700"
string(22) "1777777777777774136700"
string(30) "        1777777777777774136700"
string(4) "0-7]"
string(1) "o"

-- Iteration 6 --
string(9) "575360400"
string(1) "o"
string(9) "575360400"
string(1) "o"
string(10) " 575360400"
string(10) "575360400 "
string(10) "	575360400"
string(10) "
575360400"
string(9) "575360400"
string(30) "                     575360400"
string(4) "0-7]"
string(1) "o"

-- Iteration 7 --
string(22) "1777777777770431233000"
string(1) "o"
string(22) "1777777777770431233000"
string(1) "o"
string(23) " 1777777777770431233000"
string(23) "1777777777770431233000 "
string(23) "	1777777777770431233000"
string(23) "
1777777777770431233000"
string(22) "1777777777770431233000"
string(30) "        1777777777770431233000"
string(4) "0-7]"
string(1) "o"

-- Iteration 8 --
string(7) "4002620"
string(1) "o"
string(7) "4002620"
string(1) "o"
string(8) " 4002620"
string(8) "4002620 "
string(8) "	4002620"
string(8) "
4002620"
string(7) "4002620"
string(30) "                       4002620"
string(4) "0-7]"
string(1) "o"
Done
