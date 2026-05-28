--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - success - begins with double slashes when the URI contains a host
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("example.com");
$builder->setPath("//foo/bar/baz");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(26) "//example.com//foo/bar/baz"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "//foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}