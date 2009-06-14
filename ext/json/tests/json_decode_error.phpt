--TEST--
Test json_decode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("json")) {
 	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
/* Prototype  : mixed json_decode  ( string $json  [, bool $assoc=false  [, int $depth=512  ]] )
 * Description: Decodes a JSON string
 * Source code: ext/json/php_json.c
 * Alias to functions: 
 */
echo "*** Testing json_decode() : error conditions ***\n";

echo "\n-- Testing json_decode() function with no arguments --\n";
var_dump( json_decode() );

echo "\n-- Testing json_decode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( json_decode('"abc"', TRUE, 512, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing json_decode() : error conditions ***

-- Testing json_decode() function with no arguments --

Warning: json_decode() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing json_decode() function with more than expected no. of arguments --

Warning: json_decode() expects at most 3 parameters, 4 given in %s on line %d
NULL
===Done===
