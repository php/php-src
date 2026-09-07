--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - contains ignorable newline and tab characters
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("ht\ttp\ns");
$builder->setHost("example.com");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(20) "https://example.com/"
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
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
