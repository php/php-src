--TEST--
Test parsing URIs when a base URI is present
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com/path/to/file2", "https://test.com"));
var_dump(Uri\Rfc3986\Uri::parse("/path/to/file2", "https://test.com"));
var_dump(Uri\WhatWg\Url::parse("http://example.com/path/to/file1", "https://test.com"));
var_dump(Uri\WhatWg\Url::parse("/path/to/file1", "https://test.com"));

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["userinfo"]=>
  NULL
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
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  NULL
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
object(Uri\WhatWg\Url)#%d (%d) {
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
object(Uri\WhatWg\Url)#%d (%d) {
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
