--TEST--
Test Uri\WhatWg\Url parsing - password - multibyte codepoint
--FILE--
<?php

$url = new Uri\WhatWg\Url("http://user:pąss@example.com");

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
  string(9) "p%C4%85ss"
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
string(34) "http://user:p%C4%85ss@example.com/"
