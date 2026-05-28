--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - success - ignored for file scheme
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("file");
$builder->setPath("C:/a.txt");
$builder->setPort(443);
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(16) "file:///C:/a.txt"
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
  string(9) "/C:/a.txt"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
