--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - http scheme classification
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme("http")
    ->setHost("example.com")
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->isSpecialScheme());

?>
--EXPECTF--
string(19) "http://example.com/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
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
bool(true)
