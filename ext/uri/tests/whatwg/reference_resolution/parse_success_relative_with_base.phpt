--TEST--
Test Uri\WhatWg\Url reference resolution - parse() - relative reference with base URL
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("/with-base", new Uri\WhatWg\Url("https://example.com"));

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
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
  string(10) "/with-base"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(29) "https://example.com/with-base"
