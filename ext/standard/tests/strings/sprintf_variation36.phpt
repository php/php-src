--TEST--
Test sprintf() function : usage variations - hexa formats with resource values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : hexa formats with resource values ***\n";

// resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// array of resource types
$resource_values = array (
  $fp,
  $dfp
);

// array of hexa formats
$hexa_formats = array(  
  "%x", "%xx", "%lx", 
  "%Lx", " %x", "%x ",
  "\t%x", "\n%x", "%4x",
  "%30x", "%[0-9A-Fa-f]", "%*x"
);

$count = 1;
foreach($resource_values as $resource_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($hexa_formats as $format) {
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
*** Testing sprintf() : hexa formats with resource values ***

-- Iteration 1 --
string(%d) "%a"
string(%d) "%ax"
string(%d) "%a"
string(1) "x"
string(%d) " %a"
string(%d) "%a "
string(%d) "	%a"
string(%d) "
%a"
string(4) "%a"
string(30) "%a"
string(10) "0-9A-Fa-f]"
string(1) "x"

-- Iteration 2 --
string(%d) "%a"
string(%d) "%ax"
string(%d) "%a"
string(1) "x"
string(%d) " %a"
string(%d) "%a "
string(%d) "	%a"
string(%d) "
%a"
string(4) "%a"
string(30) "%a"
string(10) "0-9A-Fa-f]"
string(1) "x"
===DONE===
