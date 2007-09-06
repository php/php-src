--TEST--
Test addslashes() function : error conditions 
--INI--
--FILE--
<?php
/* Prototype  : string addslashes ( string $str )
 * Description: Returns a string with backslashes before characters that need to be quoted in database queries etc.
 * Source code: ext/standard/string.c
*/

/*
 * Testing addslashes() for error conditions
*/

echo "*** Testing addslashes() : error conditions ***\n";

// Zero argument
echo "\n-- Testing addslashes() function with Zero arguments --\n";
var_dump( addslashes() );

// More than expected number of arguments
echo "\n-- Testing addslashes() function with more than expected no. of arguments --\n";
$str = '"hello"\"world"';
$extra_arg = 10;

var_dump( addslashes($str, $extra_arg) );
var_dump( $str );

echo "Done\n";
?>
--EXPECTF--
*** Testing addslashes() : error conditions ***

-- Testing addslashes() function with Zero arguments --

Warning: Wrong parameter count for addslashes() in %s on line %d
NULL

-- Testing addslashes() function with more than expected no. of arguments --

Warning: Wrong parameter count for addslashes() in %s on line %d
NULL
string(15) ""hello"\"world""
Done
