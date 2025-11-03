--TEST--
Test Uri\WhatWg\Url parsing - scheme - IANA scheme
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("chrome-extension://example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(30) "chrome-extension://example.com"
