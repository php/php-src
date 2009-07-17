--TEST--
Test curl_version() function : error conditions
--SKIPIF--
<?php 
if (!extension_loaded("curl") || false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) {
	die('skip - curl extension not available in this build'); 
}
?>
--FILE--
<?php

/* Prototype  : array curl_version  ([ int $age  ] )
 * Description: Returns information about the cURL version.
 * Source code: ext/curl/interface.c
*/

echo "*** Testing curl_version() : error conditions ***\n";

echo "\n-- Testing curl_version() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( curl_version(1, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing curl_version() : error conditions ***

-- Testing curl_version() function with more than expected no. of arguments --

Warning: curl_version() expects at most 1 parameter, 2 given in %s on line %d
NULL
===Done===
