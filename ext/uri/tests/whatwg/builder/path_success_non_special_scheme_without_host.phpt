--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - non-special scheme without host
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('scheme')
    ->setPath('newPath')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(14) "scheme:newPath"
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
  string(7) "newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
