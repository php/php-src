--TEST--
Test Uri\Rfc3986\Uri parsing - path - numeric relative reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("192.168/contact.html");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(20) "192.168/contact.html"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "192.168/contact.html"
string(20) "192.168/contact.html"
