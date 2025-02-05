--TEST--
Test serialization
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUri1 = serialize($uri1);
$uri2 = unserialize($serializedUri1);

var_dump($serializedUri1);
var_dump($uri2);

$url1 = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUrl1 = serialize($url1);
$url2 = unserialize($serializedUrl1);

var_dump($serializedUrl1);
var_dump($url2);

?>
--EXPECTF--
string(145) "O:15:"Uri\Rfc3986\Uri":1:{s:5:"__uri";s:98:"https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  string(17) "username:password"
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(29) "pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
string(144) "O:14:"Uri\WhatWg\Url":1:{s:5:"__uri";s:98:"https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(29) "pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
