--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - success - basic
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setQuery("foo=1&bar=baz");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(14) "?foo=1&bar=baz"
object(Uri\Rfc3986\Uri)#2 (8) {
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
  string(13) "foo=1&bar=baz"
  ["fragment"]=>
  NULL
}
