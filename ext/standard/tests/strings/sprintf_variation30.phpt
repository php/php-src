--TEST--
Test sprintf() function : usage variations - octal formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// array of octal formats
$octal_formats = array( 
  "%o", "%ho", "%lo", 
  "%Lo", " %o", "%o ",                        
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $resource_value) );
  }
  $count++;
};

// closing the resources
fclose($fp);
closedir($dfp);

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : octal formats with resource values ***

-- Iteration 1 --
string(%d) "%d"
string(1) "o"
string(%d) "%d"
string(1) "o"
string(%d) " %d"
string(%d) "%d "
string(%d) "	%d"
string(%d) "
%d"
string(%d) "%s%d"
string(%d) "%s%d"
string(%d) "0-7]"
string(1) "o"

-- Iteration 2 --
string(%d) "%d"
string(1) "o"
string(%d) "%d"
string(1) "o"
string(%d) " %d"
string(%d) "%d "
string(%d) "	%d"
string(%d) "
%d"
string(%d) "%s%d"
string(%d) "%s%d"
string(%d) "0-7]"
string(1) "o"
Done
