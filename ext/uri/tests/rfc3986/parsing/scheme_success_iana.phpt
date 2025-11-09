--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - IANA scheme
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("chrome-extension://example.com");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
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
  NULL
  ["fragment"]=>
  NULL
}
string(30) "chrome-extension://example.com"
string(30) "chrome-extension://example.com"
