--TEST--
Test Uri\Rfc3986\Uri reference resolution - resolve() - absolute URI
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com");
$uri2 = $uri1->resolve("https://test.com/foo");

var_dump($uri1->toRawString());

var_dump($uri2);
var_dump($uri2->toRawString());
var_dump($uri2->toString());

?>
--EXPECTF--
string(19) "https://example.com"
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
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://test.com/foo"
string(20) "https://test.com/foo"
