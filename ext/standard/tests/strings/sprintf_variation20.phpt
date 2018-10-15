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
--EXPECTF--
*** Testing sprintf() : string formats with boolean values ***

-- Iteration 1 --
string(1) "1"
string(1) "s"
string(1) "1"
string(1) "s"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 2 --
string(0) ""
string(1) "s"
string(0) ""
string(1) "s"
string(1) " "
string(1) " "
string(1) "	"
string(1) "
"
string(4) "    "
string(30) "                              "
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 3 --
string(1) "1"
string(1) "s"
string(1) "1"
string(1) "s"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 4 --
string(0) ""
string(1) "s"
string(0) ""
string(1) "s"
string(1) " "
string(1) " "
string(1) "	"
string(1) "
"
string(4) "    "
string(30) "                              "
string(10) "a-zA-Z0-9]"
string(1) "s"
Done
