--TEST--
Test Uri\WhatWg\Url parsing - query - multibyte code points
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com?foo=Helló Wörld");

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
  string(27) "foo=Hell%C3%B3%20W%C3%B6rld"
  ["fragment"]=>
  NULL
}
string(47) "http://example.com/?foo=Hell%C3%B3%20W%C3%B6rld"
