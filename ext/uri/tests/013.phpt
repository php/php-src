--TEST--
Test parsing of query strings
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld"));
var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foo=Helló Wörld"));

var_dump(Uri\WhatWg\Url::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld"));
var_dump(Uri\WhatWg\Url::parse("http://example.com?foo=Helló Wörld"));

var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foobar=%27%3Cscript%3E+%2B+%40"));
var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foobar='<script> + @"));

var_dump(Uri\WhatWg\Url::parse("http://example.com?foobar=%27%3Cscript%3E+%2B+%40"));
var_dump(Uri\WhatWg\Url::parse("http://example.com?foobar='<script> + @"));

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
  string(0) ""
  ["query"]=>
  string(25) "foo=Hell%C3%B3+W%C3%B6rld"
  ["fragment"]=>
  NULL
}
NULL
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
  string(25) "foo=Hell%C3%B3+W%C3%B6rld"
  ["fragment"]=>
  NULL
}
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
  string(27) "foo=Hell%C3%B3%20W%C3%B6rld"
  ["fragment"]=>
  NULL
}
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
  string(0) ""
  ["query"]=>
  string(30) "foobar=%27%3Cscript%3E+%2B+%40"
  ["fragment"]=>
  NULL
}
NULL
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
  string(30) "foobar=%27%3Cscript%3E+%2B+%40"
  ["fragment"]=>
  NULL
}
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
