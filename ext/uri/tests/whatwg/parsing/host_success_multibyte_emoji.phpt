--TEST--
Test Uri\WhatWg\Url parsing - host - emoji codepoint
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://🐘.com");

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
  string(12) "xn--go8h.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(21) "https://xn--go8h.com/"
string(17) "https://🐘.com/"
