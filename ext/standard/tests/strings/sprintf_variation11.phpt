--TEST--
Test sprintf() function : usage variations - float formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : float formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// various float formats
$float_formats = array(
  "%f", "%hf", "%lf", 
  "%Lf", " %f", "%f ", 
  "\t%f", "\n%f", "%4f",
  "%30f", "%[0-9]", "%*f"
);

$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($float_formats as $format) {
    // with two arguments
    var_dump( sprintf($format, $resource_value) );
  }
  $count++;
};

// closing the resources
fclose($fp);
fclose($dfp);

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : float formats with resource values ***

-- Iteration 1 --
string(%d) "%d.000000"
string(1) "f"
string(%d) "%d.000000"
string(1) "f"
string(%d) " %d.000000"
string(%d) "%d.000000 "
string(%d) "	%d.000000"
string(%d) "
%d.000000"
string(%d) "%d.000000"
string(%d) "                      %d.000000"
string(4) "0-9]"
string(1) "f"

-- Iteration 2 --
string(%d) "%d.000000"
string(1) "f"
string(%d) "%d.000000"
string(1) "f"
string(%d) " %d.000000"
string(%d) "%d.000000 "
string(%d) "	%d.000000"
string(%d) "
%d.000000"
string(%d) "%d.000000"
string(%d) "                      %d.000000"
string(4) "0-9]"
string(1) "f"
Done
