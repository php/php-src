--TEST--
Test Uri\Rfc3986\Uri parsing - host - IPvFuture
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://[v7.host]/source");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());
var_dump($uri->getRawHost());
var_dump($uri->getHost());

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
  string(7) "/source"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(24) "https://[v7.host]/source"
string(24) "https://[v7.host]/source"
string(9) "[v7.host]"
string(9) "[v7.host]"
