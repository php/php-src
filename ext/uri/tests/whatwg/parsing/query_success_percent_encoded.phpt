--TEST--
Test Uri\WhatWg\Url parsing - query - percent-encoded bytes
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld");

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
  string(25) "foo=Hell%C3%B3+W%C3%B6rld"
  ["fragment"]=>
  NULL
}
string(45) "http://example.com/?foo=Hell%C3%B3+W%C3%B6rld"
