--TEST--
Test Uri\WhatWg\Url parsing - path - null byte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/pa\0th\0");

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
  string(8) "/pa%00th"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(27) "https://example.com/pa%00th"
