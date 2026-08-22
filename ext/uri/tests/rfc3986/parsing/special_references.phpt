--TEST--
Test Uri\Rfc3986\Uri parsing - special references
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("//host123/"));
var_dump(Uri\Rfc3986\Uri::parse("///foo/"));
var_dump(Uri\Rfc3986\Uri::parse("/page:1"));

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(7) "host123"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/foo/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(7) "/page:1"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
