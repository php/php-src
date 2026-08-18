--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - wss scheme classification
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme("wss")
    ->setHost("example.com")
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->isSpecialScheme());

?>
--EXPECTF--
string(18) "wss://example.com/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "wss"
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
