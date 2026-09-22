--TEST--
Test Uri\Rfc3986\Uri parsing - query - percent-encoded special characters
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http://example.com?foobar=%27%3Cscript%3E+%2B+%40");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

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
  string(30) "foobar=%27%3Cscript%3E+%2B+%40"
  ["fragment"]=>
  NULL
}
string(49) "http://example.com?foobar=%27%3Cscript%3E+%2B+%40"
string(49) "http://example.com?foobar=%27%3Cscript%3E+%2B+%40"
