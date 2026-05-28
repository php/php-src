--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - non-special scheme
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");

$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(6) "foo://"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "foo"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
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