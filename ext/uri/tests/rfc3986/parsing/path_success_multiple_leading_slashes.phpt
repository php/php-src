--TEST--
Test Uri\Rfc3986\Uri parsing - path - multiple leading slashes after an empty authority
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("http://////www.EXAMPLE.com:80");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(22) "////www.EXAMPLE.com:80"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(29) "http://////www.EXAMPLE.com:80"
string(29) "http://////www.EXAMPLE.com:80"
