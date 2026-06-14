--TEST--
Test Uri\WhatWg\Url parsing - basic - mailto email
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("mailto:user@example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(16) "user@example.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(23) "mailto:user@example.com"
