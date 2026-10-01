--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - begins with double slashes when the opaque URL contains a host
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("scheme");
$builder->setHost("example.com");
$builder->setPath("//foo/bar/baz");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(33) "scheme://example.com//foo/bar/baz"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "//foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
