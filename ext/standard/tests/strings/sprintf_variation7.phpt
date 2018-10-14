--TEST--
Test sprintf() function : usage variations - int formats with boolean values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : integer formats with boolean values ***\n";

// array of boolean type values
$boolean_values = array (
  true,
  false,
  TRUE,
  FALSE,
);

// various integer formats
$int_formats = array(
  "%d", "%hd", "%ld",
  "%Ld", " %d", "%d ",
  "\t%d", "\n%d", "%4d",
  "%30d", "%[0-9]", "%*d"
);

$count = 1;
foreach($boolean_values as $boolean_value) {
  echo "\n-- Iteration $count --\n";

  foreach($int_formats as $format) {
    var_dump( sprintf($format, $boolean_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : integer formats with boolean values ***

-- Iteration 1 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 2 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 3 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 4 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"
Done
