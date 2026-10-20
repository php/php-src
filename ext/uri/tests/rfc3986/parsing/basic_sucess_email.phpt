--TEST--
Test Uri\Rfc3986\Uri parsing - basic - mailto email
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("mailto:user@example.com");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(16) "user@example.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(23) "mailto:user@example.com"
string(23) "mailto:user@example.com"
