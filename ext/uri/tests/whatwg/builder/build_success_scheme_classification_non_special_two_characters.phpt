--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - two-character non-special scheme classification
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme("id")
    ->setHost("example.com")
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->isSpecialScheme());

?>
--EXPECTF--
string(16) "id://example.com"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(2) "id"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(false)
