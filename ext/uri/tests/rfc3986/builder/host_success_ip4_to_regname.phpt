--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - invalid IPv4 address falls back to a registered name
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("192.168.%30.1");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri->getHostType());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(15) "//192.168.%30.1"
enum(Uri\Rfc3986\UriHostType::RegisteredName)
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(13) "192.168.%30.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
