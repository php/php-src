--TEST--
Test Uri\WhatWg\Url parsing - path - percent-encoded byte
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/foo/bar%2Fbaz");

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
  string(14) "/foo/bar%2Fbaz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(33) "https://example.com/foo/bar%2Fbaz"
