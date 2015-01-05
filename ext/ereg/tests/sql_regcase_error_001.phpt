--TEST--
Test sql_regcase() function : error conditions 
--FILE--
<?php
/* Prototype  : proto string sql_regcase(string string)
 * Description: Make regular expression for case insensitive match 
 * Source code: ext/standard/reg.c
 * Alias to functions: msql_regcase
 */

echo "*** Testing sql_regcase() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing sql_regcase() function with Zero arguments --\n";
var_dump( sql_regcase() );

//Test sql_regcase with one more than the expected number of arguments
echo "\n-- Testing sql_regcase() function with more than expected no. of arguments --\n";
$string = 'string_val';
$extra_arg = 10;
var_dump( sql_regcase($string, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing sql_regcase() : error conditions ***

-- Testing sql_regcase() function with Zero arguments --

Deprecated: Function sql_regcase() is deprecated in %s on line %d

Warning: sql_regcase() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing sql_regcase() function with more than expected no. of arguments --

Deprecated: Function sql_regcase() is deprecated in %s on line %d

Warning: sql_regcase() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
