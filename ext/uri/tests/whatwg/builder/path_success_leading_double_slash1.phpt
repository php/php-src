--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - begins with double slashes when the host is not present
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("file");
$builder->setPath("//foo/bar/baz");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(20) "file:////foo/bar/baz"
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
  string(13) "//foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
