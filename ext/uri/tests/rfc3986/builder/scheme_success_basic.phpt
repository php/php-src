--TEST--
Test Uri\Rfc3986\UriBuilder::setScheme() - success - basic
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setScheme("scheme");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(7) "scheme:"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
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
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
