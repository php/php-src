--TEST--
Test sprintf() function : usage variations - octal formats with boolean values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with boolean values ***\n";

// array of boolean values 
$boolean_values = array(
  true,
  false,
  TRUE,
  FALSE,
);

// array of octal formats
$octal_formats = array( 
  "%o", "%ho", "%lo", 
  "%Lo", " %o", "%o ",                        
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($boolean_values as $boolean_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $boolean_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : octal formats with boolean values ***

-- Iteration 1 --
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

-- Iteration 2 --
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
Done