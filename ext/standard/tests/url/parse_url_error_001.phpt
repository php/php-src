--TEST--
Test parse_url() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

echo "*** Testing parse_url() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing parse_url() function with Zero arguments --\n";
var_dump( parse_url() );

//Test parse_url with one more than the expected number of arguments
echo "\n-- Testing parse_url() function with more than expected no. of arguments --\n";
$url = 'string_val';
$url_component = 10;
$extra_arg = 10;
var_dump( parse_url($url, $url_component, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing parse_url() : error conditions ***

-- Testing parse_url() function with Zero arguments --

Warning: parse_url() expects at least 1 parameter, 0 given in %s on line 12
NULL

-- Testing parse_url() function with more than expected no. of arguments --

Warning: parse_url() expects at most 2 parameters, 3 given in %s on line 19
NULL
Done
