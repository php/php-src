--TEST--
Parse multibyte URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

$url = Uri\WhatWg\Url::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor");
var_dump($url);
var_dump($url->getAsciiHost());
var_dump($url->getUnicodeHost());
var_dump($url->toAsciiString());
var_dump($url->toUnicodeString());

?>
--EXPECTF--
NULL
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(18) "xn--hostname-b1aaa"
  ["port"]=>
  int(9090)
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(14) "arg=va%C3%A9ue"
  ["fragment"]=>
  string(6) "anchor"
}
string(18) "xn--hostname-b1aaa"
string(14) "héééostname"
string(75) "http://username:password@xn--hostname-b1aaa:9090/path?arg=va%C3%A9ue#anchor"
string(71) "http://username:password@héééostname:9090/path?arg=va%C3%A9ue#anchor"
