--TEST--
Test sprintf() function : usage variations - string formats with array values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

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
--EXPECTF--
*** Testing sprintf() : string formats with array values ***

-- Iteration 1 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 2 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 3 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 4 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 5 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 6 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 7 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 8 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 9 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 10 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 11 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 12 --
string(5) "Array"
string(1) "s"
string(5) "Array"
string(1) "s"
string(6) " Array"
string(6) "Array "
string(6) "	Array"
string(6) "
Array"
string(5) "Array"
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"
Done