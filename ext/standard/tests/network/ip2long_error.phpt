--TEST--
Test ip2long() function : error conditions
--FILE--
<?php
/* Prototype  : int ip2long(string ip_address)
 * Description: Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address
 * Source code: ext/standard/basic_functions.c
 * Alias to functions:
 */

echo "*** Testing ip2long() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ip2long() function with Zero arguments --\n";
var_dump( ip2long() );

//Test ip2long with one more than the expected number of arguments
echo "\n-- Testing ip2long() function with more than expected no. of arguments --\n";
$ip_address = '127.0.0.1';
$extra_arg = 10;
var_dump( ip2long($ip_address, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing ip2long() : error conditions ***

-- Testing ip2long() function with Zero arguments --

Warning: ip2long() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ip2long() function with more than expected no. of arguments --

Warning: ip2long() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
