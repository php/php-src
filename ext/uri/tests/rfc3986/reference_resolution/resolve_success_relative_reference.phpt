--TEST--
Test Uri\Rfc3986\Uri::resolve() - success - relative reference
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com");
$uri2 = $uri1->resolve("/foo/");

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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/foo/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(24) "https://example.com/foo/"
string(24) "https://example.com/foo/"
