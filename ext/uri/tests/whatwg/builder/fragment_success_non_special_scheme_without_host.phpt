--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - non-special scheme without host
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('scheme')
    ->setFragment('newFragment')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(19) "scheme:#newFragment"
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
  NULL
  ["fragment"]=>
  string(11) "newFragment"
}
bool(true)
