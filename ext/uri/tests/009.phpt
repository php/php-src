--TEST--
Test parsing with IANA schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse("chrome-extension://example.com"));

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
