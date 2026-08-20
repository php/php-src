--TEST--
Test Uri\Rfc3986\Uri parsing - path - percent-encoded slash character
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com/foo/bar%2Fbaz");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECT--
object(Uri\Rfc3986\Uri)#1 (8) {
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
  string(14) "/foo/bar%2Fbaz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(33) "https://example.com/foo/bar%2Fbaz"
string(33) "https://example.com/foo/bar%2Fbaz"
