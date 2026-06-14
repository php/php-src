--TEST--
Test Uri\WhatWg\Url parsing - password - null byte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://username:\0pass@example.com/");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECT--
object(Uri\WhatWg\Url)#1 (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(7) "%00pass"
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
string(37) "https://username:%00pass@example.com/"
