--TEST--
Test Uri\WhatWg\Url parsing - username - codepoint in percent-encode set
--FILE--
<?php

$url = new Uri\WhatWg\Url("http://us[er]:pass@example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  string(10) "us%5Ber%5D"
  ["password"]=>
  string(4) "pass"
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
string(35) "http://us%5Ber%5D:pass@example.com/"
