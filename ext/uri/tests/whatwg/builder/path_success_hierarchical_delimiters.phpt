--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - question mark and hashmark in a hierarchical path
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("example.com");
$builder->setPath("/a?b#c");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(29) "https://example.com/a%3Fb%23c"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "/a%3Fb%23c"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
