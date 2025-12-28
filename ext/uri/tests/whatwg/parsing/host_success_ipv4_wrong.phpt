--TEST--
Test Uri\WhatWg\Url parsing - host - wrong IPv4 format
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://192.168");

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
  string(11) "192.0.0.168"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://192.0.0.168/"
