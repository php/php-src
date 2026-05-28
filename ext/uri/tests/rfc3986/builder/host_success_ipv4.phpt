--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - IPv4 address
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("192.168.0.1");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(13) "//192.168.0.1"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "192.168.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
