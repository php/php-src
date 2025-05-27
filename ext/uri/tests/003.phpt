--TEST--
Parse URL exotic URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse("http://username:password@héééostname:9090/gah/../path?arg=vaéue#anchor"));

var_dump(Uri\WhatWg\Url::parse("/page:1"));

?>
--EXPECTF--
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
NULL
