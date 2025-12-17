--TEST--
Test Uri\Rfc3986\Uri parsing - path - relative reference
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("/foo");

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
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(4) "/foo"
string(4) "/foo"
