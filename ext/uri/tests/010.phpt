--TEST--
Test parsing URIs when a base URI is present
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("http://example.com/path/to/file2", "https://test.com"));
var_dump(Uri\Rfc3986Uri::create("/path/to/file2", "https://test.com"));
var_dump(Uri\WhatWgUri::create("http://example.com/path/to/file1", "https://test.com"));
var_dump(Uri\WhatWgUri::create("/path/to/file1", "https://test.com"));

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "path/to/file2"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "path/to/file2"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "path/to/file1"
  ["query"]=>
  NULL
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
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "path/to/file1"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
