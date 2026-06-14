--TEST--
Test Uri\Rfc3986\Uri parsing - port - empty
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http://example.com:");

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
  NULL
  ["fragment"]=>
  NULL
}
string(19) "http://example.com:"
string(19) "http://example.com:"
