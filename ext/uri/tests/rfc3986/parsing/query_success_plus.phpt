--TEST--
Test Uri\Rfc3986\Uri parsing - query - plus character
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http://example.com?foo=foo+bar");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  string(11) "foo=foo+bar"
  ["fragment"]=>
  NULL
}
string(30) "http://example.com?foo=foo+bar"
string(30) "http://example.com?foo=foo+bar"
