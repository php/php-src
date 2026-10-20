--TEST--
Test Uri\WhatWg\Url parsing - username - null byte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://user\0name:pass@example.com/");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECT--
object(Uri\WhatWg\Url)#1 (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(11) "user%00name"
  ["password"]=>
  string(4) "pass"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(37) "https://user%00name:pass@example.com/"
