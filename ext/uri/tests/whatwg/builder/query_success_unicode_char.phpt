--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - contains Unicode character
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("example.com");
$builder->setQuery("főő");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(34) "https://example.com/?f%C5%91%C5%91"
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
  string(13) "f%C5%91%C5%91"
  ["fragment"]=>
  NULL
}
bool(true)
