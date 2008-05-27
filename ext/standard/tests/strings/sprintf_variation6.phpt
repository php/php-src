--TEST--
Test sprintf() function : usage variations - int formats with arrays
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : integer formats with arrays ***\n";

// different arrays used to test the function
$array_types = array (
  array(),
  array(0),
  array(1),
  array(-123),
  array("123"),
  array("-123"),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array(1 => "One", "two" => 2)
);

// various integer formats
$int_formats = array(
  "%d", "%hd", "%ld",
  "%Ld", " %d", "%d ",
  "\t%d", "\n%d", "%4d", 
  "%30d", "%[0-9]", "%*d"
);
 
$count = 1;
foreach($array_types as $arr) {
  echo "\n-- Iteration $count --\n";
  
  foreach($int_formats as $format) {
    var_dump( sprintf($format, $arr) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : integer formats with arrays ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "d"
unicode(1) "0"
unicode(1) "d"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 3 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 4 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 5 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 6 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 7 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 8 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 9 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 10 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 11 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 12 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 13 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 14 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"

-- Iteration 15 --
unicode(1) "1"
unicode(1) "d"
unicode(1) "1"
unicode(1) "d"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "d"
Done
