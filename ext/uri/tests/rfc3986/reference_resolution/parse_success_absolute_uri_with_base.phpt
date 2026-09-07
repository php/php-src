--TEST--
Test Uri\Rfc3986\Uri::parse() - success - absolute URI with base URI
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://test.com/with-base-in-vain", new Uri\Rfc3986\Uri("https://example.com"));

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
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(18) "/with-base-in-vain"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(34) "https://test.com/with-base-in-vain"
string(34) "https://test.com/with-base-in-vain"
