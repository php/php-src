--TEST--
Test HTTP URL validation
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986Uri::parse("https:example.com");
var_dump($uri);

$uri = Uri\WhatWgUri::parse("https:example.com");
var_dump($uri);

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
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
  string(11) "example.com"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
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
