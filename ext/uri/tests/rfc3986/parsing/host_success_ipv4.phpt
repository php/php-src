--TEST--
Test Uri\Rfc3986\Uri parsing - host - IPv4
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://192.168.0.1");

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
  string(11) "192.168.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(19) "https://192.168.0.1"
string(19) "https://192.168.0.1"
