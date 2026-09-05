--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setQuery("foo=bar");
$url = $builder->build(new Uri\WhatWg\Url("https://example.com/?baz"));

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(28) "https://example.com/?foo=bar"
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
  string(1) "/"
  ["query"]=>
  string(7) "foo=bar"
  ["fragment"]=>
  NULL
}
bool(true)
