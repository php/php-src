--TEST--
Test Uri\WhatWg\Url parsing - query - null byte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/?fo\0=bar\0");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECT--
object(Uri\WhatWg\Url)#1 (8) {
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
  string(1) "/"
  ["query"]=>
  string(9) "fo%00=bar"
  ["fragment"]=>
  NULL
}
string(30) "https://example.com/?fo%00=bar"
