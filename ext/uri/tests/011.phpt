--TEST--
Test encoding and normalization
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("HttPs://0300.0250.0000.0001/path?query=foo%20bar"));

var_dump(Uri\WhatWgUri::create("HttPs://0300.0250.0000.0001/path?query=foo%20bar"));

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "HttPs"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(19) "0300.0250.0000.0001"
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "1.0.168.192"
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
