--TEST--
Test strtok() function : error conditions 
--FILE--
<?php
/* Prototype  : string strtok ( string $str, string $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() for error conditions
*/

echo "*** Testing strtok() : error conditions ***\n";

// Zero argument
echo "\n-- Testing strtok() function with Zero arguments --\n";
var_dump( strtok() );

// More than expected number of arguments
echo "\n-- Testing strtok() function with more than expected no. of arguments --\n";
$str = 'sample string';
$token = ' ';
$extra_arg = 10;

var_dump( strtok($str, $token, $extra_arg) );
var_dump( $str );

// Less than expected number of arguments 
echo "\n-- Testing strtok() with less than expected no. of arguments --\n";
$str = 'string val';
 
var_dump( strtok($str));
var_dump( $str );

echo "Done\n";
?>
--EXPECTF--
*** Testing strtok() : error conditions ***

-- Testing strtok() function with Zero arguments --

Warning: strtok() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing strtok() function with more than expected no. of arguments --

Warning: strtok() expects at most 2 parameters, 3 given in %s on line %d
NULL
string(13) "sample string"

-- Testing strtok() with less than expected no. of arguments --
bool(false)
string(10) "string val"
Done
