--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - replaces existing fragment with base URL containing opaque path
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme:opaquePath?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setFragment('newFragment')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('#newFragment', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(38) "scheme:opaquePath?oldQuery#newFragment"
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
  string(10) "opaquePath"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  string(11) "newFragment"
}
bool(true)
bool(true)
