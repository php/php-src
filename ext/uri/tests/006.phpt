--TEST--
Test successful manual Uri child instance creation
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986Uri("https://username:password@example.com:8080/path?q=r#fragment");
var_dump($uri);

$uri = new Uri\WhatWgUri("https://username:password@example.com:8080/path?q=r#fragment");
var_dump($uri);

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(3) "q=r"
  ["fragment"]=>
  string(8) "fragment"
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(3) "q=r"
  ["fragment"]=>
  string(8) "fragment"
}
