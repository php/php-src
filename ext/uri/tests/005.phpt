--TEST--
Parse multibyte URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromRfc3986("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));
var_dump(\Uri\Uri::fromWhatWg("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

?>
--EXPECTF--
NULL
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(18) "xn--hostname-b1aaa"
  ["port"]=>
  int(9090)
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(14) "arg=va%C3%A9ue"
  ["fragment"]=>
  string(6) "anchor"
}
