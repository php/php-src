--TEST--
Test parse_url() function: url component specifier out of range
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components 
 * Source code: ext/standard/url.c
 * Alias to functions: 
 */

echo "*** Testing parse_url() : error conditions: url component specifier out of range ***\n";
$url = 'http://secret:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123';

echo "--> Below range:";
var_dump(parse_url($url, -1));

echo "\n\n--> Above range:";
var_dump(parse_url($url, 99));

echo "Done"
?>
--EXPECTF--
*** Testing parse_url() : error conditions: url component specifier out of range ***
--> Below range:array(8) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(6) "secret"
  ["pass"]=>
  string(7) "hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}


--> Above range:
Warning: parse_url(): Invalid URL component identifier 99 in %s on line 15
bool(false)
Done