--TEST--
Test Uri\WhatWg\Url parsing - query - plus character
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com?foo=foo+bar");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(11) "foo=foo+bar"
  ["fragment"]=>
  NULL
}
string(31) "http://example.com/?foo=foo+bar"
