--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - IPv6 address with a leading tab
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme("https")
    ->setHost("\t[::1]")
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(14) "https://[::1]/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(5) "[::1]"
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
