--TEST--
Test Uri\Rfc3986\Uri serialization and unserialization - success
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri(
    "https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"
);

$serializedUri1 = serialize($uri1);
var_dump($serializedUri1);

$uri2 = unserialize($serializedUri1);
var_dump($uri2);
var_dump($uri2->toRawString());
var_dump($uri2->toString());

?>
--EXPECTF--
string(164) "O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:99:"https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}i:1;a:0:{}}"
object(Uri\Rfc3986\Uri)#%d (%d) {
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
string(99) "https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"
