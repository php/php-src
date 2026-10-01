--TEST--
Test Uri\Rfc3986\Uri parsing - host - empty in a network-path reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("///foo/");

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
string(7) "///foo/"
string(7) "///foo/"
