--TEST--
Test parsing with IANA schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromWhatWg("chrome-extension://example.com"));
var_dump(\Uri\Uri::fromRfc3986("chrome-extension://example.com"));

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(16) "chrome-extension"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
