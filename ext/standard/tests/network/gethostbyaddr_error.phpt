--TEST--
Test gethostbyaddr() function : error conditions
--FILE--
<?php
echo "Testing gethostbyaddr : error conditions\n";

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

-- Testing gethostbyaddr function with invalid addresses --

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)

Warning: gethostbyaddr\(\): Address is not (in a.b.c.d form|a valid IPv4 or IPv6 address) in .* on line \d+
bool\(false\)
Done
