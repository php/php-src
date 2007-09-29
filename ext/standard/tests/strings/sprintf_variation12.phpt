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
--EXPECTF--
*** Testing sprintf() : float formats with arrays ***

-- Iteration 1 --
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

-- Iteration 2 --
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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
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

-- Iteration 9 --
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

-- Iteration 10 --
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

-- Iteration 11 --
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

-- Iteration 12 --
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

-- Iteration 13 --
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

-- Iteration 14 --
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
Done