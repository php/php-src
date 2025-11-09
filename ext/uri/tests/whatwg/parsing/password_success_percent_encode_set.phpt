--TEST--
Test Uri\WhatWg\Url parsing - password - codepoint in percent-encode set
--FILE--
<?php

$url = new Uri\WhatWg\Url("http://user:pa|s@example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(6) "pa%7Cs"
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
string(31) "http://user:pa%7Cs@example.com/"
