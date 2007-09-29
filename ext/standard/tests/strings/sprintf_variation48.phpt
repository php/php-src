--TEST--
Test sprintf() function : usage variations - scientific formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : scientific formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// array of scientific formats
$scientific_formats = array( 
  "%e", "%he", "%le",
  "%Le", " %e", "%e ",
  "\t%e", "\n%e", "%4e", 
  "%30e", "%[0-1]", "%*e"
);

$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($scientific_formats as $format) {
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
*** Testing sprintf() : scientific formats with resource values ***

-- Iteration 1 --
string(%d) "%d.000000e+0"
string(1) "e"
string(%d) "%d.000000e+0"
string(1) "e"
string(%d) " %d.000000e+0"
string(%d) "%d.000000e+0 "
string(%d) "	%d.000000e+0"
string(%d) "
%d.000000e+0"
string(%d) "%d.000000e+0"
string(%d) "                   %d.000000e+0"
string(4) "0-1]"
string(1) "e"

-- Iteration 2 --
string(%d) "%d.000000e+0"
string(1) "e"
string(%d) "%d.000000e+0"
string(1) "e"
string(%d) " %d.000000e+0"
string(%d) "%d.000000e+0 "
string(%d) "	%d.000000e+0"
string(%d) "
%d.000000e+0"
string(%d) "%d.000000e+0"
string(%d) "                   %d.000000e+0"
string(4) "0-1]"
string(1) "e"
Done
