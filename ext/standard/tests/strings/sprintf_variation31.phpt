--TEST--
Test sprintf() function : usage variations - octal formats with array values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with array values ***\n";

// different arrays used to test the function
$array_values = array(
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
  array(0123),
  array(1 => "One", "two" => 2)
);

// array of octal formats
$octal_formats = array(
  "%o", "%ho", "%lo",
  "%Lo", " %o", "%o ",
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($array_values as $array_value) {
  echo "\n-- Iteration $count --\n";

  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $array_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : octal formats with array values ***

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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
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

-- Iteration 9 --
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

-- Iteration 10 --
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

-- Iteration 11 --
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

-- Iteration 12 --
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

-- Iteration 13 --
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
Done
