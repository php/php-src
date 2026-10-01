--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - with base URL with opaque path
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setFragment("foo");
$url = $builder->build(new Uri\WhatWg\Url("scheme:opaque-path"));

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(22) "scheme:opaque-path#foo"
object(Uri\WhatWg\Url)#%d (%d) {
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
  string(11) "opaque-path"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(3) "foo"
}
bool(true)
