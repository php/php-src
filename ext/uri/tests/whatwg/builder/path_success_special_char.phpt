--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - contains a C0 control character
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("scheme");
$builder->setPath("\x1Ffoo");

$url = $builder->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(13) "scheme:%1Ffoo"
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
  string(6) "%1Ffoo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
