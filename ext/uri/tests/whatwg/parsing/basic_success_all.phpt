--TEST--
Test Uri\WhatWg\Url parsing - basic - all components
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "info"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/foo/bar"
  ["query"]=>
  string(15) "abc=123&def=ghi"
  ["fragment"]=>
  string(8) "hashmark"
}
string(62) "https://user:info@example.com/foo/bar?abc=123&def=ghi#hashmark"
