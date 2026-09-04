--TEST--
Test Uri\Rfc3986\Uri parsing - host - octal-like registered name
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://0300.0250.0000.0001");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());
var_dump($uri->getHostType());

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
  string(19) "0300.0250.0000.0001"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(27) "https://0300.0250.0000.0001"
string(27) "https://0300.0250.0000.0001"
enum(Uri\Rfc3986\UriHostType::RegisteredName)
