--TEST--
Test Uri\Rfc3986\Uri parsing - host - octal-like registered name
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());
var_dump($uri->getHostType());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "HttPs"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(19) "0300.0250.0000.0001"
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
string(48) "HttPs://0300.0250.0000.0001/path?query=foo%20bar"
string(48) "https://0300.0250.0000.0001/path?query=foo%20bar"
enum(Uri\Rfc3986\UriHostType::RegisteredName)
