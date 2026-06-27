--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - IPv6 address
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setScheme("https")
    ->setHost("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(39) "https://[2001:db8:85a3::8a2e:370:7334]/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(30) "[2001:db8:85a3::8a2e:370:7334]"
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
