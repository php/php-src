--TEST--
Test ezmlm_hash() function : error conditions
--FILE--
<?php
/* Prototype  : int ezmlm_hash  ( string $addr  )
 * Description: Calculate the hash value needed by EZMLM.
 * Source code: ext/standard/mail.c
 */

echo "*** Testing ezmlm_hash() : error conditions ***\n";

echo "\n-- Testing ezmlm_hash() function with fewer than expected no. of arguments --\n";
var_dump( ezmlm_hash() );

echo "\n-- Testing ezmlm_hash() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( ezmlm_hash("webmaster@example.com", $extra_arg) );

echo "\n-- Testing ezmlm_hash() function with invalid input - ARRAY --\n";
$array_arg = array(1,2,3,4);
$extra_arg = 10;
var_dump( ezmlm_hash($array_arg) );

echo "\n-- Testing ezmlm_hash() function with invalid input - OBJECT without 'cast_object' method --\n";
class sample  {
}

$obj_arg = new sample();
var_dump( ezmlm_hash($obj_arg) );

echo "\n-- Testing ezmlm_hash() function with invalid input - RESOURCE --\n";
$file_handle = fopen(__FILE__, "r");
$extra_arg = 10;
var_dump( ezmlm_hash($file_handle) );
fclose($file_handle); 

?>
===DONE===
--EXPECTF--
*** Testing ezmlm_hash() : error conditions ***

-- Testing ezmlm_hash() function with fewer than expected no. of arguments --

Warning: ezmlm_hash() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ezmlm_hash() function with more than expected no. of arguments --

Warning: ezmlm_hash() expects exactly 1 parameter, 2 given in %s on line %d
NULL

-- Testing ezmlm_hash() function with invalid input - ARRAY --

Warning: ezmlm_hash() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Testing ezmlm_hash() function with invalid input - OBJECT without 'cast_object' method --

Warning: ezmlm_hash() expects parameter 1 to be string, object given in %s on line %d
NULL

-- Testing ezmlm_hash() function with invalid input - RESOURCE --

Warning: ezmlm_hash() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===
