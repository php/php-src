--TEST--
Test Uri\Rfc3986\UriBuilder all components - success - calling reset() afterwards
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder()
    ->setScheme("https")
    ->setUserInfo("user:info")
    ->setHost("example.com")
    ->setPort(443)
    ->setPath("/foo/bar/baz")
    ->setQuery("foo=1&bar=baz")
    ->setFragment("fragment");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

$uri = $builder->reset()->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(68) "https://user:info@example.com:443/foo/bar/baz?foo=1&bar=baz#fragment"
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
  string(12) "/foo/bar/baz"
  ["query"]=>
  string(13) "foo=1&bar=baz"
  ["fragment"]=>
  string(8) "fragment"
}
string(0) ""
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
