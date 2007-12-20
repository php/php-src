--TEST--
Test sprintf() function : usage variations - unsigned formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : unsigned formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// array of unsigned formats
$unsigned_formats = array(
  "%u", "%hu", "%lu",
  "%Lu", " %u", "%u ", 
  "\t%u", "\n%u", "%4u",
   "%30u", "%[0-9]", "%*u"
);


$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($unsigned_formats as $format) {
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
*** Testing sprintf() : unsigned formats with resource values ***

-- Iteration 1 --
string(%d) "%d"
string(1) "u"
string(%d) "%d"
string(1) "u"
string(%d) " %d"
string(%d) "%d "
string(%d) "	%d"
string(%d) "
%d"
string(%d) "%s%d"
string(%d) "%s%d"
string(%d) "0-9]"
string(1) "u"

-- Iteration 2 --
string(%d) "%d"
string(1) "u"
string(%d) "%d"
string(1) "u"
string(%d) " %d"
string(%d) "%d "
string(%d) "	%d"
string(%d) "
%d"
string(%d) "%s%d"
string(%d) "%s%d"
string(%d) "0-9]"
string(1) "u"
Done
