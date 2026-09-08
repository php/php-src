--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - empty string
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("example.com");
$builder->setQuery("foo");
$builder->setQuery("");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(21) "https://example.com/?"
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
  string(0) ""
  ["fragment"]=>
  NULL
}
bool(true)
