--TEST--
Test Uri\WhatWg\Url parsing - query - emoji code points
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com/?🐘=🐘");

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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(25) "%F0%9F%90%98=%F0%9F%90%98"
  ["fragment"]=>
  NULL
}
string(46) "https://example.com/?%F0%9F%90%98=%F0%9F%90%98"
