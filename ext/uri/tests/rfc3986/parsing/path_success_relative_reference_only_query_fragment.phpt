--TEST--
Test Uri\Rfc3986\Uri parsing - path - relative reference
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("?query#fragment");

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
  string(0) ""
  ["query"]=>
  string(5) "query"
  ["fragment"]=>
  string(8) "fragment"
}
string(15) "?query#fragment"
string(15) "?query#fragment"
