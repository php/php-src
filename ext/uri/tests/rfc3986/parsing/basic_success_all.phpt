--TEST--
Test Uri\Rfc3986\Uri parsing - basic - all components
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "info"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(443)
  ["path"]=>
  string(8) "/foo/bar"
  ["query"]=>
  string(15) "abc=123&def=ghi"
  ["fragment"]=>
  string(8) "hashmark"
}
string(66) "https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark"
string(66) "https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark"
