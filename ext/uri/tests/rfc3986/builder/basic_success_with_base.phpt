--TEST--
Test Uri\Rfc3986\UriBuilder basic - success - with base URL
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder()
    ->setPath("/foo/bar/baz");
$uri = $builder->build(new Uri\Rfc3986\Uri("https://example.com"));

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(31) "https://example.com/foo/bar/baz"
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
  string(12) "/foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
