--TEST--
Test sprintf() function : usage variations - string formats with array values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

error_reporting(E_ALL & ~E_NOTICE);

echo "*** Testing sprintf() : string formats with array values ***\n";

// different arrays used to test he function
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

// array of string formats
$string_formats = array( 
  "%s", "%hs", "%ls", 
  "%Ls"," %s", "%s ", 
  "\t%s", "\n%s", "%4s",
  "%30s", "%[a-zA-Z0-9]", "%*s"
);

$count = 1;
foreach($array_values as $array_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($string_formats as $format) {
    var_dump( sprintf($format, $array_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : string formats with array values ***

-- Iteration 1 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 2 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 3 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 4 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 5 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 6 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 7 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 8 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 9 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 10 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 11 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 12 --
unicode(5) "Array"
unicode(1) "s"
unicode(5) "Array"
unicode(1) "s"
unicode(6) " Array"
unicode(6) "Array "
unicode(6) "	Array"
unicode(6) "
Array"
unicode(5) "Array"
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"
Done
