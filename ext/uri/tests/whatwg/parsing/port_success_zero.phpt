--TEST--
Test Uri\WhatWg\Url parsing - port - zero
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com:0");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(0)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(21) "http://example.com:0/"
