--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - success - empty opaque host
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("scheme");
$builder->setHost("");
$builder->setUsername("user");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(9) "scheme://"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
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
