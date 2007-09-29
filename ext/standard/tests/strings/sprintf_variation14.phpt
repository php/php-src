--TEST--
Test sprintf() function : usage variations - float formats with boolean values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : float formats with boolean values ***\n";

// array of boolean type values
$boolean_values = array (
  true,
  false,
  TRUE,
  FALSE,
);

// various float formats
$float_formats = array(
  "%f", "%hf", "%lf", 
  "%Lf", " %f", "%f ", 
  "\t%f", "\n%f", "%4f",
  "%30f", "%[0-9]", "%*f"
);

$count = 1;
foreach($boolean_values as $boolean_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($float_formats as $format) {
    var_dump( sprintf($format, $boolean_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : float formats with boolean values ***

-- Iteration 1 --
string(8) "1.000000"
string(1) "f"
string(8) "1.000000"
string(1) "f"
string(9) " 1.000000"
string(9) "1.000000 "
string(9) "	1.000000"
string(9) "
1.000000"
string(8) "1.000000"
string(30) "                      1.000000"
string(4) "0-9]"
string(1) "f"

-- Iteration 2 --
string(8) "0.000000"
string(1) "f"
string(8) "0.000000"
string(1) "f"
string(9) " 0.000000"
string(9) "0.000000 "
string(9) "	0.000000"
string(9) "
0.000000"
string(8) "0.000000"
string(30) "                      0.000000"
string(4) "0-9]"
string(1) "f"

-- Iteration 3 --
string(8) "1.000000"
string(1) "f"
string(8) "1.000000"
string(1) "f"
string(9) " 1.000000"
string(9) "1.000000 "
string(9) "	1.000000"
string(9) "
1.000000"
string(8) "1.000000"
string(30) "                      1.000000"
string(4) "0-9]"
string(1) "f"

-- Iteration 4 --
string(8) "0.000000"
string(1) "f"
string(8) "0.000000"
string(1) "f"
string(9) " 0.000000"
string(9) "0.000000 "
string(9) "	0.000000"
string(9) "
0.000000"
string(8) "0.000000"
string(30) "                      0.000000"
string(4) "0-9]"
string(1) "f"
Done

--UEXPECTF--
*** Testing sprintf() : float formats with boolean values ***

-- Iteration 1 --
unicode(8) "1.000000"
unicode(1) "f"
unicode(8) "1.000000"
unicode(1) "f"
unicode(9) " 1.000000"
unicode(9) "1.000000 "
unicode(9) "	1.000000"
unicode(9) "
1.000000"
unicode(8) "1.000000"
unicode(30) "                      1.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 2 --
unicode(8) "0.000000"
unicode(1) "f"
unicode(8) "0.000000"
unicode(1) "f"
unicode(9) " 0.000000"
unicode(9) "0.000000 "
unicode(9) "	0.000000"
unicode(9) "
0.000000"
unicode(8) "0.000000"
unicode(30) "                      0.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 3 --
unicode(8) "1.000000"
unicode(1) "f"
unicode(8) "1.000000"
unicode(1) "f"
unicode(9) " 1.000000"
unicode(9) "1.000000 "
unicode(9) "	1.000000"
unicode(9) "
1.000000"
unicode(8) "1.000000"
unicode(30) "                      1.000000"
unicode(4) "0-9]"
unicode(1) "f"

-- Iteration 4 --
unicode(8) "0.000000"
unicode(1) "f"
unicode(8) "0.000000"
unicode(1) "f"
unicode(9) " 0.000000"
unicode(9) "0.000000 "
unicode(9) "	0.000000"
unicode(9) "
0.000000"
unicode(8) "0.000000"
unicode(30) "                      0.000000"
unicode(4) "0-9]"
unicode(1) "f"
Done