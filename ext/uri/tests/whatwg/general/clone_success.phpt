--TEST--
Test cloning Uri\WhatWg\Url
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = clone $url1;

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
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://example.com/"
