--TEST--
Test Uri\WhatWg\Url parsing - path - special characters
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/fo[o/ba]r/baz=q@z,&");

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
  string(20) "/fo[o/ba]r/baz=q@z,&"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(39) "https://example.com/fo[o/ba]r/baz=q@z,&"
