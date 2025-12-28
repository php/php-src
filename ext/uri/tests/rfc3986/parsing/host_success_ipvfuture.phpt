--TEST--
Test Uri\Rfc3986\Uri parsing - host - IPvFuture
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://[v7.host]");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(9) "[v7.host]"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(17) "https://[v7.host]"
string(17) "https://[v7.host]"
