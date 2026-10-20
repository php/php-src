--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - empty string with file scheme
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('file')
    ->setPassword('')
    ->setHost('example.net')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(19) "file://example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.net"
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
