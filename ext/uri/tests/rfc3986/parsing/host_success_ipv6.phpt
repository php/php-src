--TEST--
Test Uri\Rfc3986\Uri parsing - host - IPv6
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://[2001:0db8:3333:4444:5555:6666:7777:8888]");

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
  string(41) "[2001:0db8:3333:4444:5555:6666:7777:8888]"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
