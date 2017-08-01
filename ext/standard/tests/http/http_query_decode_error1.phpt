--TEST--
Test http_query_decode() function : non-default arg_separator.input specified
--INI--
arg_separator.input = "/" 
--FILE--
<?php

/* Prototype:   array http_query_decode(string $query)
 * Description: Parses the string into variables
 * Source code: ext/standard/http.c
*/

echo "*** Testing http_query_decode() : error conditions ***\n";

echo "\n-- Testing http_query_decode() function with less than expected no. of arguments --\n";
http_query_decode();
echo "\n-- Testing http_query_decode() function with more than expected no. of arguments --\n";
$s1 = "first=val1&second=val2&third=val3";
http_query_decode($s1, [], true); 

?>
===DONE===
--EXPECTF--
*** Testing http_query_decode() : error conditions ***

-- Testing http_query_decode() function with less than expected no. of arguments --

Warning: http_query_decode() expects exactly 1 parameter, 0 given in %s on line %d

-- Testing http_query_decode() function with more than expected no. of arguments --

Warning: http_query_decode() expects exactly 1 parameter, 3 given in %s on line %d
===DONE===
