--TEST--
Test property mutation
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = clone $uri1;
var_dump($uri1);
var_dump($uri2);

$uri3 = Uri\WhatWg\Url::parse("https://example.com");
$uri4 = clone $uri3;
var_dump($uri3);
var_dump($uri4);

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#1 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#2 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#3 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#4 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
