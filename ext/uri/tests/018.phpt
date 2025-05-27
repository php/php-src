--TEST--
Test property mutation
--EXTENSIONS--
uri
--FILE--
<?php

$url3 = Uri\WhatWg\Url::parse("https://example.com");
$url4 = clone $url3;
var_dump($url3);
var_dump($url4);

?>
--EXPECTF--
object(Uri\WhatWg\Url)#1 (%d) {
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
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#2 (%d) {
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
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
