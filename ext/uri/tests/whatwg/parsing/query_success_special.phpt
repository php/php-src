--TEST--
Test Uri\WhatWg\Url parsing - query - special characters
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com?foobar='<script> + @");

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
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(30) "foobar=%27%3Cscript%3E%20+%20@"
  ["fragment"]=>
  NULL
}
string(50) "http://example.com/?foobar=%27%3Cscript%3E%20+%20@"
