--TEST--
Test Uri\WhatWg\Url parsing - username - multibyte codepoint
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://usĕr:pass@example.com");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(9) "us%C4%95r"
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
string(35) "https://us%C4%95r:pass@example.com/"
