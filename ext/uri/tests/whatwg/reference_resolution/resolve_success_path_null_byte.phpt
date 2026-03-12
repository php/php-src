--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - null byte in path
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->resolve("/f\0o");

var_dump($url1);
var_dump($url2);
var_dump($url2->toAsciiString());

?>
--EXPECT--
object(Uri\WhatWg\Url)#1 (8) {
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
object(Uri\WhatWg\Url)#2 (8) {
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
  string(6) "/f%00o"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(25) "https://example.com/f%00o"
