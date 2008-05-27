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
--EXPECT--
*** Testing sprintf() : octal formats with array values ***

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

-- Iteration 5 --
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

-- Iteration 6 --
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

-- Iteration 7 --
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

-- Iteration 8 --
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

-- Iteration 9 --
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

-- Iteration 10 --
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

-- Iteration 11 --
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

-- Iteration 12 --
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

-- Iteration 13 --
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
Done
