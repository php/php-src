--TEST--
Parse multibyte URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

$uri = Uri\WhatWg\Url::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor");
var_dump($uri);
var_dump($uri->toString());

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
string(75) "http://username:password@xn--hostname-b1aaa:9090/path?arg=va%C3%A9ue#anchor"
