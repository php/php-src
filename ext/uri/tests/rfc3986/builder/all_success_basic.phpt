--TEST--
Test Uri\Rfc3986\UriBuilder all components - success - basic
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setScheme("https");
$builder->setUserInfo("user:info");
$builder->setHost("example.com");
$builder->setPort(443);
$builder->setPath("/foo/bar/baz");
$builder->setQuery("foo=1&bar=baz");
$builder->setFragment("fragment");
$uri = $builder->build();

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
