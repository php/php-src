--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - contains digit & special characters
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("my-12+34.scheme");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(18) "my-12+34.scheme://"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(15) "my-12+34.scheme"
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
