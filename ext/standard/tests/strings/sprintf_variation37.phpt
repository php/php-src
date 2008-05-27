--TEST--
Test sprintf() function : usage variations - hexa formats with array values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : hexa formats with array values ***\n";

// array of array values 
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
  array(1 => "One", "two" => 2)
);

// array of hexa formats
$hexa_formats = array(  
  "%x", "%xx", "%lx", 
  "%Lx", " %x", "%x ",
  "\t%x", "\n%x", "%4x",
  "%30x", "%[0-9A-Fa-f]", "%*x"
);

$count = 1;
foreach($array_values as $array_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($hexa_formats as $format) {
    var_dump( sprintf($format, $array_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : hexa formats with array values ***

-- Iteration 1 --
unicode(1) "0"
unicode(2) "0x"
unicode(1) "0"
unicode(1) "x"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 2 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 3 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 4 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 5 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 6 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 7 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 8 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 9 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 10 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 11 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"

-- Iteration 12 --
unicode(1) "1"
unicode(2) "1x"
unicode(1) "1"
unicode(1) "x"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "0-9A-Fa-f]"
unicode(1) "x"
Done
