--TEST--
Test str_replace() function basic function
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string
*/

echo "\n*** Testing str_replace() on basic operations ***\n";

var_dump( str_replace("", "", "") );

var_dump( str_replace("e", "b", "test") );

var_dump( str_replace("", "", "", $count) );
var_dump( $count );

var_dump( str_replace("q", "q", "q", $count) );
var_dump( $count );

var_dump( str_replace("long string here", "", "", $count) );
var_dump( $count );

$fp = fopen( __FILE__, "r" );
$fp_copy = $fp; 
var_dump( str_replace($fp_copy, $fp_copy, $fp_copy, $fp_copy) );
var_dump( $fp_copy );
fclose($fp);

?>
===DONE===
--EXPECTF--	
*** Testing str_replace() on basic operations ***
string(0) ""
string(4) "tbst"
string(0) ""
int(0)
string(1) "q"
int(1)
string(0) ""
int(0)
string(%d) "Resource id #%d"
int(1)
===DONE===
