--TEST--
Test normalization
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri = new Uri\WhatWgUri("https://EXAMPLE.COM/foo/../bar");
var_dump($uri->normalize());
exit;

$uri1 = Uri\Rfc3986Uri::parse("HTTPS://////EXAMPLE.com");
$uri2 = $uri1->normalize();
$uri3 = Uri\Rfc3986Uri::parse("https://example.com")->normalize();
$uri4 = Uri\Rfc3986Uri::parse("https://example.com/foo/..")->normalize();

var_dump($uri1);
var_dump($uri2);
var_dump($uri3->__toString());
var_dump($uri4->__toString());

$uri5 = Uri\WhatWgUri::parse("HTTPS://////EXAMPLE.com");
$uri6 = $uri5->normalize();
$uri7 = Uri\WhatWgUri::parse("https://example.com")->normalize();
$uri8 = Uri\WhatWgUri::parse("https://example.com/foo/..")->normalize();

var_dump($uri5);
var_dump($uri6);
var_dump($uri7->__toString());
var_dump($uri8->__toString());

?>
--EXPECT--
object(Uri\Rfc3986Uri)#1 (%d) {
  ["scheme"]=>
  string(5) "HTTPS"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(14) "///EXAMPLE.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986Uri)#2 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(14) "/example.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(19) "https://example.com"
string(19) "https://example.com"
object(Uri\WhatWgUri)#5 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#6 (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://example.com/"
string(20) "https://example.com/"
