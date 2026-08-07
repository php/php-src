--TEST--
Test Uri\WhatWg\UrlBuilder all components - success - calling reset() afterwards
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setScheme("https")
    ->setUsername("user")
    ->setPassword("pass")
    ->setHost("example.com")
    ->setPort(444)
    ->setPath("/foo/bar/baz")
    ->setQuery("foo=1&bar=baz")
    ->setFragment("fragment");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

$url = $builder
    ->reset()
    ->setScheme("scheme")
    ->setHost("www.example.com")
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(68) "https://user:pass@example.com:444/foo/bar/baz?foo=1&bar=baz#fragment"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "pass"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(444)
  ["path"]=>
  string(12) "/foo/bar/baz"
  ["query"]=>
  string(13) "foo=1&bar=baz"
  ["fragment"]=>
  string(8) "fragment"
}
bool(true)
string(24) "scheme://www.example.com"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
