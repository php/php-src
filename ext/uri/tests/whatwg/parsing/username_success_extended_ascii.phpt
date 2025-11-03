--TEST--
Test Uri\WhatWg\Url parsing - username - extended ASCII character
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://úzör@example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(14) "%C3%BAz%C3%B6r"
  ["password"]=>
  string(0) ""
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
string(35) "https://%C3%BAz%C3%B6r@example.com/"
