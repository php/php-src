--TEST--
Test Uri\WhatWg\UrlBuilder basic - success - with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setPath("/foo/bar/baz");
$url = $builder->build(new Uri\WhatWg\Url("https://example.com"));

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(31) "https://example.com/foo/bar/baz"
object(Uri\WhatWg\Url)#%d (%d) {
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
