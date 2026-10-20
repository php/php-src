--TEST--
Test Uri\WhatWg\Url parsing - path - multibyte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/fȎȎ");

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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(14) "/f%C8%8E%C8%8E"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(33) "https://example.com/f%C8%8E%C8%8E"
