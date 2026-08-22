--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4 address in octal notation
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar");

var_dump($url);
var_dump($url->toUnicodeString());
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
  string(11) "192.168.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
string(40) "https://192.168.0.1/path?query=foo%20bar"
string(40) "https://192.168.0.1/path?query=foo%20bar"
