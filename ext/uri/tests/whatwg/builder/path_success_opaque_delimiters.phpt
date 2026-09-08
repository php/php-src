--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - question mark and hashmark in an opaque path
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setPath("a?b#c");
$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(13) "foo:a%3Fb%23c"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "foo"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(9) "a%3Fb%23c"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
