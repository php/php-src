--TEST--
Test parsing of various schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("mailto:Joe@Example.COM"));
var_dump(Uri\WhatWgUri::create("mailto:Joe@Example.COM"));

var_dump(Uri\Rfc3986Uri::create("file:///E:/Documents%20and%20Settings"));
var_dump(Uri\WhatWgUri::create("file:///E:\\Documents and Settings"));

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(15) "Joe@Example.COM"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(15) "Joe@Example.COM"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(29) "E:/Documents%20and%20Settings"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(29) "E:/Documents%20and%20Settings"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
