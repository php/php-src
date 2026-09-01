--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4 address in octal notation
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://0300.0250.0000.0001");

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
string(20) "https://192.168.0.1/"
