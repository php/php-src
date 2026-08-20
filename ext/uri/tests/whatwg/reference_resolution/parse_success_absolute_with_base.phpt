--TEST--
Test Uri\WhatWg\Url reference resolution - parse() - absolute URL with base URL
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://test.com/with-base-in-vain", Uri\WhatWg\Url::parse("https://example.com/"));

var_dump($url);
var_dump($url->toUnicodeString());
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
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(18) "/with-base-in-vain"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(34) "https://test.com/with-base-in-vain"
string(34) "https://test.com/with-base-in-vain"
