--TEST--
Test Uri\Rfc3986\UriBuilder::setPort() - success - large number
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPort(PHP_INT_MAX);
$builder->setHost("example.com");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(%d) "//example.com:%d"
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
  int(%d)
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
