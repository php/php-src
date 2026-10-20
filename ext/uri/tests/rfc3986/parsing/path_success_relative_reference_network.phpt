--TEST--
Test Uri\Rfc3986\Uri parsing - path - network-path reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("//host123/");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

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
  string(7) "host123"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(10) "//host123/"
string(10) "//host123/"
