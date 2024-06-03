--TEST--
Test Uri::withSchema()
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\Uri(null, "example.com", null, null, null, null, null, null);

$url2 = $url->withScheme("https");

var_dump($url);
var_dump($url->getScheme());
var_dump($url2);
var_dump($url2->getScheme());

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
NULL
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(5) "https"
