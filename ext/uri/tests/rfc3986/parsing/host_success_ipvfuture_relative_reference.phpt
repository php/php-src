--TEST--
Test Uri\Rfc3986\Uri parsing - host - IPvFuture in a relative reference
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("//[v7.host]/source");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

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
  string(9) "[v7.host]"
  ["port"]=>
  NULL
  ["path"]=>
  string(7) "/source"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(18) "//[v7.host]/source"
string(18) "//[v7.host]/source"
