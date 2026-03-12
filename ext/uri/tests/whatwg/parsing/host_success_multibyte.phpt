--TEST--
Test Uri\WhatWg\Url parsing - host - multibyte codepoint
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://exḁmple.com");

var_dump($url);
var_dump($url->toAsciiString());
var_dump($url->toUnicodeString());

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
  string(19) "xn--exmple-xf7b.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(28) "https://xn--exmple-xf7b.com/"
string(22) "https://exḁmple.com/"
