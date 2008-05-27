--TEST--
Test sprintf() function : usage variations - scientific formats with boolean values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : scientific formats with boolean values ***\n";

// array of boolean values 
$boolean_values = array(
  true,
  false,
  TRUE,
  FALSE,
);

// array of scientific formats
$scientific_formats = array( 
  "%e", "%he", "%le",
  "%Le", " %e", "%e ",
  "\t%e", "\n%e", "%4e", 
  "%30e", "%[0-1]", "%*e"
);

$count = 1;
foreach($boolean_values as $boolean_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($scientific_formats as $format) {
    var_dump( sprintf($format, $boolean_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : scientific formats with boolean values ***

-- Iteration 1 --
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(12) " 1.000000e+0"
unicode(12) "1.000000e+0 "
unicode(12) "	1.000000e+0"
unicode(12) "
1.000000e+0"
unicode(11) "1.000000e+0"
unicode(30) "                   1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 2 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 3 --
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(12) " 1.000000e+0"
unicode(12) "1.000000e+0 "
unicode(12) "	1.000000e+0"
unicode(12) "
1.000000e+0"
unicode(11) "1.000000e+0"
unicode(30) "                   1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 4 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"
Done
