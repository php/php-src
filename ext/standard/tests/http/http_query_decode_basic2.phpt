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

echo "*** Testing http_query_decode() : non-default arg_separator.input specified ***\n";

$query = "first=val1/second=val2/third=val3";
var_dump(http_query_decode($query));

?>
===DONE===
--EXPECT--
*** Testing http_query_decode() : non-default arg_separator.input specified ***
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}
===DONE===
