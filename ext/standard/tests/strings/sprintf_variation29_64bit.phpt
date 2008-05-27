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
--EXPECT--
*** Testing sprintf() : octal formats with float values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 2 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 3 --
unicode(1) "1"
unicode(1) "o"
unicode(1) "1"
unicode(1) "o"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 4 --
unicode(6) "303240"
unicode(1) "o"
unicode(6) "303240"
unicode(1) "o"
unicode(7) " 303240"
unicode(7) "303240 "
unicode(7) "	303240"
unicode(7) "
303240"
unicode(6) "303240"
unicode(30) "                        303240"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 5 --
unicode(22) "1777777777777774136700"
unicode(1) "o"
unicode(22) "1777777777777774136700"
unicode(1) "o"
unicode(23) " 1777777777777774136700"
unicode(23) "1777777777777774136700 "
unicode(23) "	1777777777777774136700"
unicode(23) "
1777777777777774136700"
unicode(22) "1777777777777774136700"
unicode(30) "        1777777777777774136700"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 6 --
unicode(9) "575360400"
unicode(1) "o"
unicode(9) "575360400"
unicode(1) "o"
unicode(10) " 575360400"
unicode(10) "575360400 "
unicode(10) "	575360400"
unicode(10) "
575360400"
unicode(9) "575360400"
unicode(30) "                     575360400"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 7 --
unicode(22) "1777777777770431233000"
unicode(1) "o"
unicode(22) "1777777777770431233000"
unicode(1) "o"
unicode(23) " 1777777777770431233000"
unicode(23) "1777777777770431233000 "
unicode(23) "	1777777777770431233000"
unicode(23) "
1777777777770431233000"
unicode(22) "1777777777770431233000"
unicode(30) "        1777777777770431233000"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 8 --
unicode(7) "4002620"
unicode(1) "o"
unicode(7) "4002620"
unicode(1) "o"
unicode(8) " 4002620"
unicode(8) "4002620 "
unicode(8) "	4002620"
unicode(8) "
4002620"
unicode(7) "4002620"
unicode(30) "                       4002620"
unicode(4) "0-7]"
unicode(1) "o"
Done
