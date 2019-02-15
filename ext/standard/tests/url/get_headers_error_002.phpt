--TEST--
Test get_headers() function: wrong type for argument format
--CREDITS--
June Henriksen <juneih@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
/* Prototype  : proto array get_headers(string url[, int format])
 * Description: Fetches all the headers sent by the server in response to a HTTP request
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

echo "*** Testing get_headers() : error conditions ***\n";
$url = 'http://php.net';

// Format argument as type String
echo "\n-- Testing get_headers() function with format argument as type string --\n";
var_dump( get_headers($url, "#PHPTestFest2009 Norway") );

// Format argument as type Array
echo "\n-- Testing get_headers() function with format argument as type array --\n";
var_dump( get_headers($url, array()) );

// Format argument as type Object
class testObject
{
}

$object = new testObject();
echo "\n-- Testing get_headers() function with format argument as type object --\n";
var_dump( get_headers($url, $object) );


echo "Done"
?>
--EXPECTF--
*** Testing get_headers() : error conditions ***

-- Testing get_headers() function with format argument as type string --

Warning: get_headers() expects parameter 2 to be int, string given in %s on line 13
NULL

-- Testing get_headers() function with format argument as type array --

Warning: get_headers() expects parameter 2 to be int, array given in %s on line 17
NULL

-- Testing get_headers() function with format argument as type object --

Warning: get_headers() expects parameter 2 to be int, object given in %s on line 26
NULL
Done
