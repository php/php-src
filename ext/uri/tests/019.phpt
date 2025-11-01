--TEST--
Test IDNA support
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://🐘.com/🐘?🐘=🐘");
var_dump($uri);

$url = Uri\WhatWg\Url::parse("https://🐘.com/🐘?🐘=🐘", null);
var_dump($url);
var_dump($url->getAsciiHost());
var_dump($url->getUnicodeHost());
var_dump($url->toAsciiString());
var_dump($url->toUnicodeString());

?>
--EXPECTF--
NULL
object(Uri\WhatWg\Url)#1 (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(12) "xn--go8h.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "/%F0%9F%90%98"
  ["query"]=>
  string(25) "%F0%9F%90%98=%F0%9F%90%98"
  ["fragment"]=>
  NULL
}
string(12) "xn--go8h.com"
string(8) "🐘.com"
string(59) "https://xn--go8h.com/%F0%9F%90%98?%F0%9F%90%98=%F0%9F%90%98"
string(55) "https://🐘.com/%F0%9F%90%98?%F0%9F%90%98=%F0%9F%90%98"
