--TEST--
Test Uri\Rfc3986\Uri reference resolution - parse() - relative reference with base URI
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("/with-base", new Uri\Rfc3986\Uri("https://example.com"));

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "/with-base"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(29) "https://example.com/with-base"
string(29) "https://example.com/with-base"
