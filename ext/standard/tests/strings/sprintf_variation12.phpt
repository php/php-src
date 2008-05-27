--TEST--
Test sprintf() function : usage variations - float formats with arrays 
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : float formats with arrays ***\n";

// array of array types
$array_values = array (
  array(),
  array(0),
  array(1),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array("123.456abc"),
  array('123.456abc'),
  array(1 => "One", "two" => 2)
);

// various float formats
$float_formats = array(
  "%f", "%hf", "%lf", 
  "%Lf", " %f", "%f ", 
  "\t%f", "\n%f", "%4f",
  "%30f", "%[0-9]", "%*f"
);

$count = 1;
foreach($array_values as $array_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($float_formats as $format) {
    // with two arguments
    var_dump( sprintf($format, $array_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : float formats with arrays ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
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

-- Iteration 9 --
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

-- Iteration 10 --
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

-- Iteration 11 --
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

-- Iteration 12 --
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

-- Iteration 13 --
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

-- Iteration 14 --
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
Done
