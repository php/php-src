--TEST--
Test cloning Uri\Rfc3986\Uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = clone $uri1;

var_dump($uri1->toRawString());

var_dump($uri2);
var_dump($uri2->toRawString());
var_dump($uri2->toString());

?>
--EXPECTF--
string(19) "https://example.com"
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
string(19) "https://example.com"
string(19) "https://example.com"
