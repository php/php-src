--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - IPv4 address
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setScheme("https")
    ->setHost("192.168.0.1");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(20) "https://192.168.0.1/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "192.168.0.1"
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
