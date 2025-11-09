--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - only username
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://user:@example.com");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(0) ""
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
string(25) "https://user:@example.com"
string(25) "https://user:@example.com"
