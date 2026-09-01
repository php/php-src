--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - absolute URL
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com");
$url2 = $url1->resolve("https://test.com/foo");

var_dump($url1->toAsciiString());

var_dump($url2);
var_dump($url2->toUnicodeString());
var_dump($url2->toAsciiString());

?>
--EXPECTF--
string(20) "https://example.com/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://test.com/foo"
string(20) "https://test.com/foo"
