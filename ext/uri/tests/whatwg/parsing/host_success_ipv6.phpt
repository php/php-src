--TEST--
Test Uri\WhatWg\Url parsing - host - IPv6
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://[2001:0db8:3333:4444:5555:6666:7777:8888]");

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
