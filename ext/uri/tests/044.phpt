--TEST--
Test handling of the userinfo component of RFC 3986
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://us%65rinfo:::@example.com");
$uri2 = $uri1->withUserInfo("");
$uri3 = $uri2->withUserInfo("userinfo");

var_dump($uri1->getUserInfo());
var_dump($uri1->getRawUserInfo());
var_dump($uri1);

var_dump($uri2->getUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2);

var_dump($uri3->getUserInfo());
var_dump($uri3->getRawUserInfo());
var_dump($uri3);

?>
--EXPECTF--
string(11) "userinfo:::"
string(13) "us%65rinfo:::"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  string(13) "us%65rinfo:::"
  ["user"]=>
  string(10) "us%65rinfo"
  ["password"]=>
  string(2) "::"
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
NULL
NULL
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  NULL
  ["user"]=>
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
string(8) "userinfo"
string(8) "userinfo"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  string(8) "userinfo"
  ["user"]=>
  string(8) "userinfo"
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
