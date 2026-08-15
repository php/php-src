--TEST--
Test Uri\Rfc3986\Uri parsing with a base URI
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com/path/to/file2", new Uri\Rfc3986\Uri("https://test.com")));
var_dump(Uri\Rfc3986\Uri::parse("/path/to/file2", new Uri\Rfc3986\Uri("https://test.com")));
?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
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
  string(14) "/path/to/file2"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(14) "/path/to/file2"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
