--TEST--
Test Uri\WhatWg\UrlBuilder basic - success - with scheme relative URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setHost("example.net");
$builder->setPath("/foo/bar/baz");
$builder->setPort(124);
$url = $builder->build(new Uri\WhatWg\Url("https://user:pass@example.com:123/foo/bar?query#hash"));

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(35) "https://example.net:124/foo/bar/baz"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  int(124)
  ["path"]=>
  string(12) "/foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
