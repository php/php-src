--TEST--
Test Uri\WhatWg\Url parsing - path - emoji codepoint
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com/🐘");

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
  string(13) "/%F0%9F%90%98"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(32) "https://example.com/%F0%9F%90%98"
