--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - IPvFuture address
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("[v1.2001:db8::1]");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(18) "//[v1.2001:db8::1]"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(16) "[v1.2001:db8::1]"
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
