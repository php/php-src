--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("192.168/contact.html"));
var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));

var_dump(Uri\Rfc3986\Uri::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
var_dump(Uri\WhatWg\Url::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(20) "192.168/contact.html"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
NULL
NULL
NULL
