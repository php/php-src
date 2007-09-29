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

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 2 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 3 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 4 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 5 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 6 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 8 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 9 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 10 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 11 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"

-- Iteration 12 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(1) "s"

Notice: Array to string conversion in %s on line %d
string(6) " Array"

Notice: Array to string conversion in %s on line %d
string(6) "Array "

Notice: Array to string conversion in %s on line %d
string(6) "	Array"

Notice: Array to string conversion in %s on line %d
string(6) "
Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(30) "                         Array"
string(10) "a-zA-Z0-9]"
string(1) "s"
Done

--UEXPECTF--
*** Testing sprintf() : string formats with array values ***

-- Iteration 1 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 2 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 3 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 4 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 5 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 6 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 8 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 9 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 10 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 11 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"

-- Iteration 12 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"
unicode(1) "s"

Notice: Array to string conversion in %s on line %d
unicode(6) " Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "Array "

Notice: Array to string conversion in %s on line %d
unicode(6) "	Array"

Notice: Array to string conversion in %s on line %d
unicode(6) "
Array"

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

Notice: Array to string conversion in %s on line %d
unicode(30) "                         Array"
unicode(10) "a-zA-Z0-9]"
unicode(1) "s"
Done
