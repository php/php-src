--TEST--
Parse multibyte URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

$url = Uri\WhatWg\Url::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor");
var_dump($url);
var_dump($url->getHost());
var_dump($url->getHostForDisplay());
var_dump($url->toDisplayString());
var_dump($url->toString());

?>
--EXPECTF--
NULL
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(18) "xn--hostname-b1aaa"
  ["port"]=>
  int(9090)
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(14) "arg=va%C3%A9ue"
  ["fragment"]=>
  string(6) "anchor"
}
string(18) "xn--hostname-b1aaa"
string(14) "héééostname"
string(71) "http://username:password@héééostname:9090/path?arg=va%C3%A9ue#anchor"
string(75) "http://username:password@xn--hostname-b1aaa:9090/path?arg=va%C3%A9ue#anchor"
