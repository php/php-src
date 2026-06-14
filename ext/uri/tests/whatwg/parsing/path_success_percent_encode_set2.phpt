--TEST--
Test Uri\WhatWg\Url parsing - path - codepoint in percent-encode set
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/foo\"/<bar>/^{baz}");

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
  string(28) "/foo%22/%3Cbar%3E/^%7Bbaz%7D"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(47) "https://example.com/foo%22/%3Cbar%3E/^%7Bbaz%7D"
