--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - IPv6 address
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setHost("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(43) "//[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(41) "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"
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
