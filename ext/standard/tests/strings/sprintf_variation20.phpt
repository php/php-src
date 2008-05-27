--TEST--
Test sprintf() function : usage variations - string formats with boolean values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : string formats with boolean values ***\n";

// array of boolean values 
$boolean_values = array(
  true,
  false,
  TRUE,
  FALSE,
);

// array of string formats
$string_formats = array( 
  "%s", "%hs", "%ls", 
  "%Ls"," %s", "%s ", 
  "\t%s", "\n%s", "%4s",
  "%30s", "%[a-zA-Z0-9]", "%*s"
);

$count = 1;
foreach($boolean_values as $boolean_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($string_formats as $format) {
    var_dump( sprintf($format, $boolean_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : string formats with boolean values ***

-- Iteration 1 --
unicode(1) "1"
unicode(1) "s"
unicode(1) "1"
unicode(1) "s"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 2 --
unicode(0) ""
unicode(1) "s"
unicode(0) ""
unicode(1) "s"
unicode(1) " "
unicode(1) " "
unicode(1) "	"
unicode(1) "
"
unicode(4) "    "
unicode(30) "                              "
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 3 --
unicode(1) "1"
unicode(1) "s"
unicode(1) "1"
unicode(1) "s"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 4 --
unicode(0) ""
unicode(1) "s"
unicode(0) ""
unicode(1) "s"
unicode(1) " "
unicode(1) " "
unicode(1) "	"
unicode(1) "
"
unicode(4) "    "
unicode(30) "                              "
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"
Done