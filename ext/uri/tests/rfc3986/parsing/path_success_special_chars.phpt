--TEST--
Test Uri\Rfc3986\Uri parsing - path - special characters
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("/foo:1@&+=");

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
  string(10) "/foo:1@&+="
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(10) "/foo:1@&+="
string(10) "/foo:1@&+="
