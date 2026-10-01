--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - empty string
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("file");
$builder->setPath("/foo/bar");
$builder->setPath("");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(8) "file:///"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
