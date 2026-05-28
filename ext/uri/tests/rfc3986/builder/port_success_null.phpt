--TEST--
Test Uri\Rfc3986\UriBuilder::setPort() - success - null
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPort(null);
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);

?>
--EXPECTF--
string(0) ""
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
  NULL
}
