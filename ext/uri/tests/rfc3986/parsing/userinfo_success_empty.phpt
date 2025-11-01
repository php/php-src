--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - empty
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://@example.com");

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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://@example.com"
string(20) "https://@example.com"
