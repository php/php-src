--TEST--
Test parsing URIs when a base URI is present
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromRfc3986("http://example.com/path/to/file2", "https://test.com"));
var_dump(\Uri\Uri::fromRfc3986("/path/to/file2", "https://test.com"));

var_dump(\Uri\Uri::fromWhatWg("http://example.com/path/to/file1", "https://test.com"));
var_dump(\Uri\Uri::fromWhatWg("/path/to/file1", "https://test.com"));

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(8) "test.com"
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
  string(5) "https"
  ["host"]=>
  string(8) "test.com"
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
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(14) "/path/to/file1"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  int(0)
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(14) "/path/to/file1"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}