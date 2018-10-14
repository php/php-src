--TEST--
Test gethostbyaddr() function : error conditions
--FILE--
<?php
/* Prototype  : proto string gethostbyaddr(string ip_address)
 * Description: Get the Internet host name corresponding to a given IP address
 * Source code: ext/standard/dns.c
 * Alias to functions:
 */


echo "Testing gethostbyaddr : error conditions\n";

// Zero arguments
echo "\n-- Testing gethostbyaddr function with Zero arguments --\n";
var_dump( gethostbyaddr() );

//Test gethostbyaddr with one more than the expected number of arguments
echo "\n-- Testing gethostbyaddr function with more than expected no. of arguments --\n";
$ip_address = 'string_val';
$extra_arg = 10;
var_dump( gethostbyaddr($ip_address, $extra_arg) );

echo "\n-- Testing gethostbyaddr function with invalid addresses --\n";

$ip_address = 'invalid';
var_dump( gethostbyaddr($ip_address) );

$ip_address = '300.1.2.3';
var_dump( gethostbyaddr($ip_address) );

$ip_address = '256.1.2.3';
var_dump( gethostbyaddr($ip_address) );

echo "Done";
?>
--EXPECTREGEX--
Testing gethostbyaddr : error conditions

-- Testing gethostbyaddr function with Zero arguments --

Warning: gethostbyaddr\(\) expects exactly 1 parameter, 0 given in .* on line \d+
NULL

-- Testing gethostbyaddr function with more than expected no. of arguments --

Warning: gethostbyaddr\(\) expects exactly 1 parameter, 2 given in .* on line \d+
NULL

-- Testing gethostbyaddr function with invalid addresses --

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)
Done
