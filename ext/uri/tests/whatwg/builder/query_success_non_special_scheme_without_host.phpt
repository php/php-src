--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - non-special scheme without host
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('scheme')
    ->setQuery('newQuery')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(16) "scheme:?newQuery"
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
  string(0) ""
  ["query"]=>
  string(8) "newQuery"
  ["fragment"]=>
  NULL
}
bool(true)
