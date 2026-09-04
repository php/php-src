--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - relative reference
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com");
$url2 = $url1->resolve("/foo/");

var_dump($url1->toAsciiString());

var_dump($url2);
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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/foo/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(24) "https://example.com/foo/"
