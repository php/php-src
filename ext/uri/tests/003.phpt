--TEST--
Parse URL exotic URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));
var_dump(Uri\WhatWgUri::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

var_dump(Uri\Rfc3986Uri::parse("/page:1"));
var_dump(Uri\WhatWgUri::parse("/page:1"));

?>
--EXPECTF--
NULL
object(Uri\WhatWgUri)#%d (%d) {
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
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(6) "page:1"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(1) {
  [0]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["position"]=>
    string(7) "/page:1"
    ["errorCode"]=>
    int(21)
  }
}
