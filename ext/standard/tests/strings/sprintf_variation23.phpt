--TEST--
Test sprintf() function : usage variations - char formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : char formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// array of char formats
$char_formats = array( 
  "%c", "%hc", "%lc", 
  "%Lc", " %c", "%c ",
  "\t%c", "\n%c", "%4c",
  "%30c", "%[a-bA-B@#$&]", "%*c"
);

$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($char_formats as $format) {
    var_dump( sprintf($format, $resource_value) );
  }
  $count++;
};

// closing the resources
fclose($fp);
closedir($dfp);

?>
===DONE===
--EXPECTF--
*** Testing sprintf() : char formats with resource values ***

-- Iteration 1 --
string(1) "%a"
string(1) "c"
string(1) "%a"
string(1) "c"
string(2) " %a"
string(2) "%a "
string(2) "	%a"
string(2) "
%a"
string(1) "%a"
string(1) "%a"
string(11) "a-bA-B@#$&]"
string(1) "c"

-- Iteration 2 --
string(1) "%a"
string(1) "%a"
string(1) "%a"
string(1) "c"
string(2) " %a"
string(2) "%a "
string(2) "	%a"
string(2) "
%a"
string(1) "%a"
string(1) "%a"
string(11) "a-bA-B@#$&]"
string(1) "c"
===DONE===
