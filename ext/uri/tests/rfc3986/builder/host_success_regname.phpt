--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - Registered name
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("www.example.com");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(17) "//www.example.com"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
