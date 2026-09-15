--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - empty fragment with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setFragment('#')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('#', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(39) "https://example.com/base/path?oldQuery#"
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
  string(10) "/base/path"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  string(0) ""
}
bool(true)
bool(true)
