--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - question mark in hierarchical path
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('https')
    ->setHost('example.com')
    ->setPath('/a?b')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(25) "https://example.com/a%3Fb"
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
  string(6) "/a%3Fb"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
