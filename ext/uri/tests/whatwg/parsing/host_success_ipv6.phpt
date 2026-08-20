--TEST--
Test Uri\WhatWg\Url parsing - host - IPv6
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($url1);
var_dump($url1->toUnicodeString());
var_dump($url1->toAsciiString());
var_dump($url1->getAsciiHost());

$url2 = new Uri\WhatWg\Url("https://[0:0::1]");

var_dump($url2);
var_dump($url2->toUnicodeString());
var_dump($url2->toAsciiString());
var_dump($url2->getAsciiHost());

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
  string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
object(Uri\WhatWg\Url)#2 (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(5) "[::1]"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(14) "https://[::1]/"
string(14) "https://[::1]/"
string(5) "[::1]"
