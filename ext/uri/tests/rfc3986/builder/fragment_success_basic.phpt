--TEST--
Test Uri\Rfc3986\UriBuilder::setFragment() - success - basic
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setFragment("foo");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(4) "#foo"
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
  NULL
  ["fragment"]=>
  string(3) "foo"
}
bool(true)
