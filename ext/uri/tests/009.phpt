--TEST--
Test parsing with IANA schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("chrome-extension://example.com"));
var_dump(Uri\WhatWgUri::create("chrome-extension://example.com"));

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
