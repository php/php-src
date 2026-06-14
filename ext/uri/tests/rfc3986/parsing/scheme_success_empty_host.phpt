--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - empty host
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https:example.com");

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
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(11) "example.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(17) "https:example.com"
string(17) "https:example.com"
