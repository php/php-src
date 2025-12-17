--TEST--
Test Uri\Rfc3986\Uri parsing - host - wrong IPv4 format
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://192.168");

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
  string(7) "192.168"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(15) "https://192.168"
string(15) "https://192.168"
