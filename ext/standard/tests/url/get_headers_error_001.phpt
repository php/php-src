--TEST--
Test get_headers() function : error conditions - wrong number of args
--CREDITS--
June Henriksen <juneih@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
/* Prototype  : proto array get_headers(string url[, int format[, resource $context]])
 * Description: Fetches all the headers sent by the server in response to a HTTP request
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

echo "*** Testing get_headers() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing get_headers() function with Zero arguments --\n";
var_dump( get_headers() );

//Test get_headers with one more than the expected number of arguments
echo "\n-- Testing get_headers() function with more than expected no. of arguments --\n";
$url       = 'string_val';
$format    = 1;
$context   = stream_context_get_default();
$extra_arg = 10;
var_dump( get_headers($url, $format, $context, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_headers() : error conditions ***

-- Testing get_headers() function with Zero arguments --

Warning: get_headers() expects at least 1 parameter, 0 given in %s on line 12
NULL

-- Testing get_headers() function with more than expected no. of arguments --

Warning: get_headers() expects at most 3 parameters, 4 given in %s on line 20
NULL
Done
