--TEST--
Test Uri\WhatWg\Url serialization and unserialization
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");

$serializedUrl1 = serialize($url1);
var_dump($serializedUrl1);

$url2 = unserialize($serializedUrl1);
var_dump($url2);
var_dump($url2->toAsciiString());

?>
--EXPECTF--
string(163) "O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:99:"https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}i:1;a:0:{}}"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
string(99) "https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"
