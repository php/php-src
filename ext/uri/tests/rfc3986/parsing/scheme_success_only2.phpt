--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - only scheme
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http:/");

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
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(6) "http:/"
string(6) "http:/"
