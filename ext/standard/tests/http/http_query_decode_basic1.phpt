--TEST--
Test http_query_decode() function : basic functionality 
--FILE--
<?php
		
/* Prototype:   array http_query_decode(string $query)
 * Description: Parses the string into variables
 * Source code: ext/standard/http.c
*/

echo "*** Testing http_query_decode() : basic functionality ***\n";

echo "Basic test\n";
$query = "first=val1&second=val2&third=val3";
var_dump(http_query_decode($query));

?>
===DONE===
--EXPECTF--
*** Testing http_query_decode() : basic functionality ***
Basic test
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}
===DONE===
