--TEST--
Test Uri\WhatWg\Url parsing - host - short IPv6
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://[0:0::1]");

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
