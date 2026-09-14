--TEST--
Test Uri\Rfc3986\Uri parsing - query - percent-encoded bytes
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld");

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
  string(25) "foo=Hell%C3%B3+W%C3%B6rld"
  ["fragment"]=>
  NULL
}
string(44) "http://example.com?foo=Hell%C3%B3+W%C3%B6rld"
string(44) "http://example.com?foo=Hell%C3%B3+W%C3%B6rld"
