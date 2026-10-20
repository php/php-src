--TEST--
Test Uri\WhatWg\Url parsing - password - extended ASCII character
--FILE--
<?php

$url = new Uri\WhatWg\Url("http://user:pásswörd@example.com");

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
  string(18) "p%C3%A1ssw%C3%B6rd"
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
string(43) "http://user:p%C3%A1ssw%C3%B6rd@example.com/"
